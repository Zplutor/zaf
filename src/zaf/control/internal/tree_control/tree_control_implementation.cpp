#include <zaf/control/internal/tree_control/tree_control_implementation.h>
#include <zaf/base/as.h>
#include <zaf/base/container/utility/contain.h>
#include <zaf/base/container/utility/find.h>
#include <zaf/base/container/utility/sort.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/base/range.h>
#include <zaf/control/internal/tree_control/utility.h>
#include <zaf/internal/list/list_control_parts_context.h>

namespace zaf::internal {
namespace {

std::vector<std::pair<std::size_t, std::size_t>> MergeAdjacentListIndexes(
    const std::vector<std::size_t>& list_indexes) {

    //Merge adjacent indexes.
    std::vector<std::pair<std::size_t, std::size_t>> index_ranges;

    std::size_t range_index = list_indexes.front();
    std::size_t range_length = 1;
    std::size_t last_index = range_index;

    for (auto i : Range::FromIndexPair(1u, list_indexes.size())) {

        auto current_index = list_indexes[i];

        if (last_index + 1 == current_index) {
            range_length++;
        }
        else {

            index_ranges.push_back(std::make_pair(range_index, range_length));

            range_index = current_index;
            range_length = 1;
        }

        last_index = current_index;
    }

    index_ranges.push_back(std::make_pair(range_index, range_length));
    return index_ranges;
}

}

TreeControlImplementation::TreeControlImplementation(ScrollBox& owner) : list_parts_(&owner) {

}


void TreeControlImplementation::Initialize(const InitializeParameters& parameters) {

    InstallDataSource(parameters.data_source);
    InstallDelegate(parameters.delegate);
    InitializeListImplementation(parameters);

    item_container_subscriptions_ += parameters.item_container->ChangeExpandStateEvent().Subscribe(
        std::bind(
            &TreeControlImplementation::OnChangeExpandStateByArrowKeys, 
            this, 
            std::placeholders::_1));

    data_source_change_event_ = parameters.data_source_change_event;
    delegate_change_event_ = parameters.delegate_change_event;
    selection_change_event_ = parameters.selection_change_event;
    item_expand_event_ = parameters.item_expand_event;
    item_collapse_event_ = parameters.item_collapse_event;

    Reload();
}


void TreeControlImplementation::InitializeListImplementation(
    const InitializeParameters& parameters) {

    //This event must be subscribed before initializing the list core.
    Subscriptions() +=
        list_parts_.SelectionStore().ChangedEvent().Subscribe(
            std::bind_front(&TreeControlImplementation::OnListSelectionStoreChanged, this));

    ListCore::InitializeParameters list_initialize_parameters;
    list_initialize_parameters.item_container = parameters.item_container;
    list_initialize_parameters.data_source = As<ListDataSource>(shared_from_this());
    list_initialize_parameters.delegate = As<ListControlDelegate>(shared_from_this());

    Subscriptions() += list_parts_.Core().SelectionChangedEvent().Subscribe(
        std::bind_front(&TreeControlImplementation::OnListSelectionChange, this));

    Subscriptions() += list_parts_.Core().ItemDoubleClickEvent().Subscribe(
        std::bind_front(&TreeControlImplementation::OnListItemDoubleClick, this));

    list_parts_.Core().Initialize(list_initialize_parameters);
}


void TreeControlImplementation::InstallDataSource(
    const std::weak_ptr<TreeDataSource>& data_source) {

    UnregisterDataSourceEvents();

    data_source_ = data_source;
    RegisterDataSourceEvents();
}


void TreeControlImplementation::RegisterDataSourceEvents() {

    auto data_source = data_source_.lock();
    if (!data_source) {
        return;
    }

    data_source_subscriptions_ += data_source->DataAddEvent().Subscribe(
        std::bind(&TreeControlImplementation::OnDataAdd, this, std::placeholders::_1));

    data_source_subscriptions_ += data_source->DataRemoveEvent().Subscribe(
        std::bind(&TreeControlImplementation::OnDataRemove, this, std::placeholders::_1));

    data_source_subscriptions_ += data_source->DataUpdateEvent().Subscribe(
        std::bind(&TreeControlImplementation::OnDataUpdate, this, std::placeholders::_1));
}


void TreeControlImplementation::UnregisterDataSourceEvents() {

    data_source_subscriptions_.Clear();
}


void TreeControlImplementation::InstallDelegate(
    const std::weak_ptr<TreeControlDelegate>& delegate) {

    delegate_ = delegate;
}


void TreeControlImplementation::SetDataSource(const std::weak_ptr<TreeDataSource>& data_source) {

    auto previous_data_source = data_source_;
    InstallDataSource(data_source);

    if (data_source_change_event_) {
        data_source_change_event_(previous_data_source.lock());
    }

    Reload();
}


void TreeControlImplementation::SetDelegate(const std::weak_ptr<TreeControlDelegate>& delegate) {

    auto previous_delegate = delegate_;
    InstallDelegate(delegate);

    if (delegate_change_event_) {
        delegate_change_event_(previous_delegate.lock());
    }

    Reload();
}


void TreeControlImplementation::Reload() {

    ReloadRootNode();

    list_parts_.Core().Reload();
}


void TreeControlImplementation::ReloadRootNode() {

    tree_index_mapping_.Clear();
    tree_data_manager_.Clear();

    ExpandItem(nullptr);
}


std::vector<std::shared_ptr<Object>> TreeControlImplementation::GetAllSelectedItems() const {

    std::vector<std::pair<std::shared_ptr<Object>, IndexPath>> selected_data_index_path;

    for (const auto& each_data : tree_data_manager_.GetAllSelectedNodes()) {

        auto index_path = tree_data_manager_.GetIndexPathOfData(each_data);
        if (!index_path) {
            continue;
        }

        selected_data_index_path.emplace_back(each_data, *index_path);
    }

    Sort(selected_data_index_path, [](const auto& pair1, const auto& pair2) {
        return pair1.second < pair2.second;
    });

    std::vector<std::shared_ptr<Object>> result;
    for (const auto& each_pair : selected_data_index_path) {
        result.push_back(each_pair.first);
    }
    return result;
}


std::shared_ptr<Object> TreeControlImplementation::GetFirstSelectedItem() const {

    auto all_selected_data = GetAllSelectedItems();
    if (!all_selected_data.empty()) {
        return all_selected_data.front();
    }
    return nullptr;
}


void TreeControlImplementation::VisitExpandedTree(TreeVisitor& visitor) const {

    auto root_node = tree_data_manager_.GetNodeAtIndexPath({});
    if (root_node) {
        VisitExpandedTreeNode(*root_node, visitor);
    }
}


void TreeControlImplementation::VisitExpandedTreeNode(
    const TreeNode& node,
    TreeVisitor& visitor) const {

    visitor.VisitNode(node.data);

    visitor.EnterNode();
    for (const auto& each_child : node.children) {
        if (each_child->is_expanded) {
            VisitExpandedTreeNode(*each_child, visitor);
        }
    }
    visitor.ExitNode();
}


void TreeControlImplementation::SelectItem(const std::shared_ptr<Object>& data) {

    auto list_index = GetDataListIndex(data);
    if (!list_index) {
        return;
    }

    list_parts_.SelectionManager().SelectItemAtIndex(*list_index);
}


void TreeControlImplementation::UnselectItem(const std::shared_ptr<Object>& data) {

    auto list_index = GetDataListIndex(data);
    if (!list_index) {
        return;
    }

    list_parts_.SelectionManager().UnselectItemAtIndex(*list_index);
}


void TreeControlImplementation::ExpandItem(const std::shared_ptr<Object>& data) {

    //Don't expand if it is already expanded.
    if (tree_data_manager_.IsNodeExpanded(data)) {
        return;
    }

    auto index_path = tree_data_manager_.GetIndexPathOfData(data);
    if (!index_path) {
        return;
    }

    auto list_index = tree_index_mapping_.GetIndexAtIndexPath(*index_path);
    ExpandItemUI(*index_path, list_index, true);
}


void TreeControlImplementation::CollapseItem(const std::shared_ptr<Object>& data) {

    //Don't collapse if it is already collapsed.
    if (!tree_data_manager_.IsNodeExpanded(data)) {
        return;
    }

    auto index_path = tree_data_manager_.GetIndexPathOfData(data);
    if (!index_path) {
        return;
    }

    auto list_index = tree_index_mapping_.GetIndexAtIndexPath(*index_path);
    CollapseItemUI(*index_path, list_index, true);
}


void TreeControlImplementation::ScrollToItem(const std::shared_ptr<Object>& data) {

    auto list_index = GetDataListIndex(data);
    if (!list_index) {
        return;
    }

    list_parts_.Core().ScrollToItemAtIndex(*list_index);
}


void TreeControlImplementation::ReloadItem(const std::shared_ptr<Object>& data) {

    auto index_path = tree_data_manager_.GetIndexPathOfData(data);
    if (!index_path) {
        return;
    }

    UpdateItem(*index_path);
}


std::size_t TreeControlImplementation::GetDataCount() const {
    return tree_index_mapping_.GetNodeCount();
}


std::shared_ptr<Object> TreeControlImplementation::GetDataAtIndex(std::size_t index) const {

    auto index_path = tree_index_mapping_.GetIndexPathAtIndex(index);
    auto tree_node = tree_data_manager_.GetNodeAtIndexPath(index_path);
    ZAF_EXPECT(tree_node);
    return tree_node->data;
}


bool TreeControlImplementation::HasVariableItemHeight() {

    auto delegate = delegate_.lock();
    if (delegate) {
        return delegate->HasVariableItemHeight();
    }
    return __super::HasVariableItemHeight();
}


float TreeControlImplementation::EstimateItemHeight(
    std::size_t item_index,
    const std::shared_ptr<Object>& item_data) {

    auto result = InnerEstimateItemHeight(item_index, item_data);
    if (result) {
        return *result;
    }
    return __super::EstimateItemHeight(item_index, item_data);
}


float TreeControlImplementation::GetItemSpacing() {

    auto delegate = delegate_.lock();
    if (delegate) {
        return delegate->GetItemSpacing();
    }
    return 0;
}


std::optional<float> TreeControlImplementation::InnerEstimateItemHeight(
    std::size_t item_index,
    const std::shared_ptr<Object>& item_data) {

    std::shared_ptr<Object> parent_data;
    std::size_t child_index{};
    if (!GetParentDataAndChildIndex(item_index, parent_data, child_index)) {
        return std::nullopt;
    }

    auto delegate = delegate_.lock();
    if (!delegate) {
        return std::nullopt;
    }
   
    return delegate->EstimateItemHeight(parent_data, child_index, item_data);
}


std::shared_ptr<ListItem> TreeControlImplementation::CreateItem(
    std::size_t item_index,
    const std::shared_ptr<Object>& item_data) {

    auto list_item = InnerCreateListItem(item_index, item_data);
    if (list_item) {
        return list_item;
    }
    return __super::CreateItem(item_index, item_data);
}


std::shared_ptr<ListItem> TreeControlImplementation::InnerCreateListItem(
    std::size_t item_index,
    const std::shared_ptr<Object>& item_data) {

    std::shared_ptr<Object> parent_data;
    std::size_t child_index{};
    if (!GetParentDataAndChildIndex(item_index, parent_data, child_index)) {
        return nullptr;
    }

    auto index_path = tree_index_mapping_.GetIndexPathAtIndex(item_index);
    if (index_path.empty()) {
        return nullptr;
    }

    auto delegate = delegate_.lock();
    if (!delegate) {
        return nullptr;
    }

    auto tree_item = delegate->CreateItem(parent_data, child_index, item_data);
    tree_item->SetTreeControlImplementation(shared_from_this());

    tree_item->SetIndentLevel(index_path.size() - 1);
    SetItemExpandState(tree_item, item_data, index_path);

    return tree_item;
}


void TreeControlImplementation::SetItemExpandState(
    const std::shared_ptr<TreeItem>& item,
    const std::shared_ptr<Object>& item_data,
    const IndexPath& index_path) {

    auto data_source = data_source_.lock();
    if (!data_source) {
        return;
    }

    if (!data_source->DoesDataHasChildren(item_data)) {
        item->SetExpandState(ExpandState::None);
    }
    else if (IsIndexPathExpanded(index_path)) {
        item->SetExpandState(ExpandState::Expanded);
    }
    else {
        item->SetExpandState(ExpandState::Collapsed);
    }
}


bool TreeControlImplementation::IsIndexPathExpanded(const IndexPath& index_path) const {

    auto tree_node = tree_data_manager_.GetNodeAtIndexPath(index_path);
    if (!tree_node) {
        return false;
    }

    return tree_node->is_expanded;
}


bool TreeControlImplementation::ExpandItemUI(
    const IndexPath& index_path,
    const std::optional<std::size_t>& list_index,
    bool update_item) {

    std::shared_ptr<Object> expanded_data;
    std::size_t expanded_count{};
    if (!ExpandItemData(index_path, expanded_data, expanded_count)) {
        return false;
    }

    if (list_index) {
        NotifyDataAdded(*list_index + 1, expanded_count);
    }

    if (update_item) {
        UpdateItem(index_path);
    }

    if (item_expand_event_) {
        item_expand_event_(expanded_data);
    }

    return true;
}


bool TreeControlImplementation::ExpandItemData(
    const IndexPath& index_path,
    std::shared_ptr<Object>& expanded_data,
    std::size_t& expanded_count) {

    auto data_source = data_source_.lock();
    if (!data_source) {
        return false;
    }

    auto node_expander = tree_data_manager_.ExpandNodeAtIndexPath(index_path);
    if (!node_expander) {
        return false;
    }

    //Check if can expand non-root item.
    if (!index_path.empty()) {

        auto delegate = delegate_.lock();
        if (!delegate) {
            return false;
        }

        std::shared_ptr<Object> parent_data;
        std::size_t index_in_parent{};
        if (!GetParentDataAndChildIndex(index_path, parent_data, index_in_parent)) {
            return false;
        }

        if (!delegate->CanExpandItem(parent_data, index_in_parent, node_expander->GetNodeData())) {
            return false;
        }
    }

    expanded_data = node_expander->GetNodeData();
    expanded_count = ExpandItemRecursively(*data_source, *node_expander, index_path);
    return true;
}


std::size_t TreeControlImplementation::ExpandItemRecursively(
    TreeDataSource& data_source,
    TreeNodeExpander& node_expander, 
    const IndexPath& node_index_path) {

    auto child_count = data_source.GetChildDataCount(node_expander.GetNodeData());

    //Expand the node with specified child count.
    node_expander.SetChildCount(child_count);
    tree_index_mapping_.AddChildren(node_index_path, 0, child_count);

    std::size_t total_child_count = child_count;

    //Process children.
    while (node_expander.MoveToNextChild()) {

        auto child_node_expander = node_expander.GetCurrentChildExpander();

        //Expand the child recursively.
        if (child_node_expander) {

            auto child_index_path = node_index_path;
            child_index_path.push_back(node_expander.GetCurrentChildIndex());

            total_child_count += ExpandItemRecursively(
                data_source, 
                *child_node_expander,
                child_index_path);
        }
        //Set data to each child.
        else {

            auto child_data = data_source.GetChildDataAtIndex(
                node_expander.GetNodeData(),
                node_expander.GetCurrentChildIndex());

            node_expander.SetDataToCurrentChild(child_data);
        }
    }

    return total_child_count;
}


bool TreeControlImplementation::CollapseItemUI(
    const IndexPath& index_path,
    const std::optional<std::size_t>& list_index,
    bool update_item) {

    auto delegate = delegate_.lock();
    if (!delegate) {
        return false;
    }

    auto tree_node = tree_data_manager_.GetNodeAtIndexPath(index_path);
    if (!tree_node) {
        return false;
    }

    std::shared_ptr<Object> parent_data;
    std::size_t index_in_parent{};
    if (!GetParentDataAndChildIndex(index_path, parent_data, index_in_parent)) {
        return false;
    }

    if (!delegate->CanCollapseItem(parent_data, index_in_parent, tree_node->data)) {
        return false;
    }

    auto removed_count = tree_index_mapping_.RemoveAllChildrenRecursively(index_path);

    auto collapse_result = tree_data_manager_.CollapseNode(tree_node->data);
    if (collapse_result.selection_changed) {
        NotifySelectionChange();
    }

    if (list_index) {
        NotifyDataRemoved(*list_index + 1, removed_count);
    }

    if (update_item) {
        UpdateItem(index_path);
    }

    if (item_collapse_event_) {
        item_collapse_event_(tree_node->data);
    }

    return true;
}


bool TreeControlImplementation::ChangeItemExpandState(
    const std::shared_ptr<TreeItem>& item, 
    bool new_is_expanded) {

    auto& visible_item_manager = list_parts_.VisibleItemManager();
    auto list_item_index = visible_item_manager.GetIndexOfVisibleItem(*item);
    if (!list_item_index) {
        return false;
    }

    auto index_path = tree_index_mapping_.GetIndexPathAtIndex(*list_item_index);
    if (index_path.empty()) {
        return false;
    }

    if (new_is_expanded) {
        return ExpandItemUI(index_path, *list_item_index, false);
    }
    else {
        return CollapseItemUI(index_path, *list_item_index, false);
    }
}


void TreeControlImplementation::OnListItemDoubleClick(
    const ListCoreItemDoubleClickInfo& event_info) {

    auto index_path = tree_index_mapping_.GetIndexPathAtIndex(event_info.item_index);
    if (index_path.empty()) {
        return;
    }

    if (IsIndexPathExpanded(index_path)) {
        CollapseItemUI(index_path, event_info.item_index, true);
    }
    else {
        ExpandItemUI(index_path, event_info.item_index, true);
    }
}


void TreeControlImplementation::OnChangeExpandStateByArrowKeys(bool is_to_expand) {

    //Support changing expand state with arrow keys in only single selection mode.
    auto selection_mode = list_parts_.SelectionManager().SelectionMode();
    if (selection_mode != SelectionMode::Single) {
        return;
    }

    auto& selection_store = list_parts_.SelectionStore();
    auto selected_index = selection_store.GetFirstSelectedIndex(); 
    if (!selected_index) {
        return;
    }

    auto index_path = tree_index_mapping_.GetIndexPathAtIndex(*selected_index);
    if (index_path.empty()) {
        return;
    }

    bool is_index_path_expanded = IsIndexPathExpanded(index_path);

    if (is_to_expand) {

        if (!is_index_path_expanded) {
            ExpandItemUI(index_path, selected_index, true);
        }
        else {
            selected_index = GetChildListIndex(index_path, 0);
            if (selected_index) {
                auto& selection_manager = list_parts_.SelectionManager();
                selection_manager.SelectItemAtIndex(*selected_index);
            }
        }
    }
    else {
        if (is_index_path_expanded) {
            CollapseItemUI(index_path, selected_index, true);
        }
        else {
            auto parent_index_path = index_path;
            parent_index_path.pop_back();
            selected_index = tree_index_mapping_.GetIndexAtIndexPath(parent_index_path);
            if (selected_index) {
                auto& selection_manager = list_parts_.SelectionManager();
                selection_manager.SelectItemAtIndex(*selected_index);
            }
        }
    }

    if (selected_index) {
        list_parts_.Core().ScrollToItemAtIndex(*selected_index);
    }
}


void TreeControlImplementation::UpdateItem(const IndexPath& index_path) {

    auto list_index = tree_index_mapping_.GetIndexAtIndexPath(index_path);
    if (!list_index) {
        return;
    }

    CheckIfItemHasChildren(index_path, *list_index);
    NotifyDataUpdated(*list_index, 1);
}


void TreeControlImplementation::CheckIfItemHasChildren(
    const IndexPath& index_path, 
    std::size_t list_index) {

    auto data_source = data_source_.lock();
    if (!data_source) {
        return;
    }

    auto tree_node = tree_data_manager_.GetNodeAtIndexPath(index_path);
    if (!tree_node) {
        return;
    }

    if (data_source->DoesDataHasChildren(tree_node->data)) {
        return;
    }

    //Remove all children if the node doesn't have children after updating.
    auto collapse_node = tree_data_manager_.CollapseNodeWithoutReserveExpandState(tree_node->data);
    if (collapse_node.selection_changed) {
        NotifySelectionChange();
    }

    auto remove_count = tree_index_mapping_.RemoveAllChildrenRecursively(index_path);
    if (remove_count <= 0) {
        return;
    }

    NotifyDataRemoved(list_index + 1, remove_count);
}


void TreeControlImplementation::OnListSelectionStoreChanged(
    const ListSelectionStoreChangedInfo& event_info) {

    switch (event_info.reason) {
    case ListSelectionChangeReason::AddSelection:
        ModifySelection(event_info.changed_range.index, event_info.changed_range.length, false);
        break;
    case ListSelectionChangeReason::ReplaceSelection:
        ModifySelection(event_info.changed_range.index, event_info.changed_range.length, true);
        break;
    case ListSelectionChangeReason::RemoveSelection:
        RemoveSelection(event_info.changed_range.index, event_info.changed_range.length);
        break;
    case ListSelectionChangeReason::ItemChange:
    default:
        return;
    }
}


void TreeControlImplementation::OnListSelectionChange(None) {
    NotifySelectionChange();
}


void TreeControlImplementation::NotifySelectionChange() {

    if (selection_change_event_) {
        selection_change_event_();
    }
}


void TreeControlImplementation::ModifySelection(
    std::size_t index, 
    std::size_t count, 
    bool is_replace) {

    if (is_replace) {
        tree_data_manager_.UnselectAllNodes();
    }

    for (auto list_index : Range(index, count)) {

        auto index_path = tree_index_mapping_.GetIndexPathAtIndex(list_index);
        if (index_path.empty()) {
            continue;
        }

        auto node = tree_data_manager_.GetNodeAtIndexPath(index_path);
        if (!node) {
            continue;
        }

        tree_data_manager_.SelectNode(node->data);
    }
}


void TreeControlImplementation::RemoveSelection(std::size_t index, std::size_t count) {

    for (auto list_index : Range(index, count)) {

        auto index_path = tree_index_mapping_.GetIndexPathAtIndex(list_index);
        if (index_path.empty()) {
            continue;
        }

        auto node = tree_data_manager_.GetNodeAtIndexPath(index_path);
        if (!node) {
            continue;
        }

        tree_data_manager_.UnselectNode(node->data);
    }
}


void TreeControlImplementation::OnDataAdd(const TreeDataSourceDataAddInfo& event_info) {

    auto data_source = data_source_.lock();
    if (!data_source) {
        return;
    }

    //Get parent index path.
    auto parent_index_path = tree_data_manager_.GetIndexPathOfData(event_info.parent_data);
    if (!parent_index_path) {
        return;
    }

    auto children_adder = tree_data_manager_.AddChildrenAtIndexPath(*parent_index_path); 
    if (children_adder) {
    
        children_adder->SetAddRange(event_info.index, event_info.count);

        //Set data to each child.
        while (children_adder->MoveToNextChild()) {

            auto child_data = data_source->GetChildDataAtIndex(
                event_info.parent_data,
                children_adder->GetCurrentChildIndex());

            children_adder->SetDataToCurrentChild(child_data);
        }

        //Get list index at which to insert new items.
        auto list_index = GetChildListIndex(*parent_index_path, event_info.index);
        if (list_index) {

            //Add children to tree ui data.
            tree_index_mapping_.AddChildren(*parent_index_path, event_info.index, event_info.count);

            //Raise event.
            NotifyDataAdded(*list_index, event_info.count);
        }
    }

    //Update parent item.
    UpdateItem(*parent_index_path);
}


std::optional<std::size_t> TreeControlImplementation::GetChildListIndex(
    const IndexPath& parent_index_path,
    std::size_t child_index) const {

    auto old_child_count = tree_index_mapping_.GetChildCount(parent_index_path);
    if (!old_child_count) {
        return std::nullopt;
    }

    //Child index must not exceed child count.
    ZAF_EXPECT(child_index <= old_child_count);

    //There is no child, get next index of parent's index.
    if (old_child_count == 0) {

        auto list_index = tree_index_mapping_.GetIndexAtIndexPath(parent_index_path);
        if (!list_index) {
            return std::nullopt;
        }
        return *list_index + 1;
    }

    if (child_index < old_child_count) {

        auto child_index_path = parent_index_path;
        child_index_path.push_back(child_index);
        return tree_index_mapping_.GetIndexAtIndexPath(child_index_path);
    }

    //Child index is in the end, get next index to last child's index.
    auto child_index_path = parent_index_path;
    child_index_path.push_back(child_index - 1);

    auto list_index = tree_index_mapping_.GetIndexAtIndexPath(child_index_path);
    if (!list_index) {
        return std::nullopt;
    }

    //Add child count recursively.
    auto recursive_child_count = tree_index_mapping_.GetChildCountRecursively(child_index_path);
    if (recursive_child_count) {
        *list_index += *recursive_child_count;
    }

    *list_index += 1;
    return list_index;
}


void TreeControlImplementation::OnDataRemove(const TreeDataSourceDataRemoveInfo& event_info) {

    //Get parent index path.
    auto parent_index_path = tree_data_manager_.GetIndexPathOfData(event_info.parent_data);
    if (!parent_index_path) {
        return;
    }

    //Remove tree nodes.
    auto remove_result = tree_data_manager_.RemoveChildren(
        event_info.parent_data,
        event_info.index,
        event_info.count);

    //Get list index at which begin removing.
    auto child_index_path = *parent_index_path;
    child_index_path.push_back(event_info.index);

    auto list_index = tree_index_mapping_.GetIndexAtIndexPath(child_index_path);
    if (list_index) {

        //Remove from ui data.
        auto total_remove_count = tree_index_mapping_.RemoveChildren(
            *parent_index_path,
            event_info.index,
            event_info.count);

        //Raise event.
        NotifyDataRemoved(*list_index, total_remove_count);
    }

    //Update parent item.
    UpdateItem(*parent_index_path);

    if (remove_result.selection_changed) {
        NotifySelectionChange();
    }
}


void TreeControlImplementation::OnDataUpdate(const TreeDataSourceDataUpdateInfo& event_info) {

    auto updated_list_indexes = UpdateChildItem(
        event_info.parent_data, 
        event_info.index,
        event_info.count);

    if (updated_list_indexes.empty()) {
        return;
    }

    auto updated_index_ranges = MergeAdjacentListIndexes(updated_list_indexes);

    //Raise events.
    for (const auto& each_range : updated_index_ranges) {
        NotifyDataUpdated(each_range.first, each_range.second);
    }
}


std::vector<std::size_t> TreeControlImplementation::UpdateChildItem(
    const std::shared_ptr<Object>& parent_data,
    std::size_t index,
    std::size_t count) {

    auto index_path = tree_data_manager_.GetIndexPathOfData(parent_data);
    if (!index_path) {
        return {};
    }

    std::vector<std::size_t> result;

    IndexPath child_index_path = *index_path;
    for (auto index : Range(index, count)) {

        child_index_path.push_back(index);

        auto list_index = tree_index_mapping_.GetIndexAtIndexPath(child_index_path);
        if (list_index) {

            CheckIfItemHasChildren(child_index_path, *list_index);

            result.push_back(*list_index);
        }

        child_index_path.pop_back();
    }

    return result;
}


std::optional<std::size_t> TreeControlImplementation::GetDataListIndex(
    const std::shared_ptr<Object>& data) const {

    auto index_path = tree_data_manager_.GetIndexPathOfData(data);
    if (!index_path) {
        return std::nullopt;
    }

    return tree_index_mapping_.GetIndexAtIndexPath(*index_path);
}


bool TreeControlImplementation::GetParentDataAndChildIndex(
    std::size_t list_index,
    std::shared_ptr<Object>& parent_data,
    std::size_t& child_index) {

    auto index_path = tree_index_mapping_.GetIndexPathAtIndex(list_index);
    if (index_path.empty()) {
        return false;
    }

    return GetParentDataAndChildIndex(index_path, parent_data, child_index);
}


bool TreeControlImplementation::GetParentDataAndChildIndex(
    const IndexPath& index_path,
    std::shared_ptr<Object>& parent_data,
    std::size_t& child_index) {

    auto data_source = data_source_.lock();
    if (!data_source) {
        return false;
    }

    child_index = index_path.back();

    auto parent_index_path = index_path;
    parent_index_path.pop_back();

    auto tree_node = tree_data_manager_.GetNodeAtIndexPath(parent_index_path);
    if (!tree_node) {
        return false;
    }

    parent_data = tree_node->data;
    return true;
}

}