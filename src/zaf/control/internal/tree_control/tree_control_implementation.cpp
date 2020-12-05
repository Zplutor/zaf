#include <zaf/control/internal/tree_control/tree_control_implementation.h>
#include <zaf/base/container/utility/contain.h>
#include <zaf/base/container/utility/find.h>
#include <zaf/base/container/utility/range.h>
#include <zaf/base/container/utility/sort.h>
#include <zaf/base/error/check.h>
#include <zaf/control/internal/tree_control/utility.h>

namespace zaf::internal {

TreeControlImplementation::TreeControlImplementation(ScrollableControl& owner) :
    list_implementation_(std::make_shared<internal::ListControlImplementation>(owner)) {

}


void TreeControlImplementation::Initialize(const InitializeParameters& parameters) {

    InstallDataSource(parameters.data_source);
    InstallDelegate(parameters.delegate);
    InitializeListImplementation(parameters);

    data_source_change_event_ = parameters.data_source_change_event;
    delegate_change_event_ = parameters.delegate_change_event;
    selection_change_event_ = parameters.selection_change_event;
    item_expand_event_ = parameters.item_expand_event;
    item_collapse_event_ = parameters.item_collapse_event;

    Reload();
}


void TreeControlImplementation::InitializeListImplementation(
    const InitializeParameters& parameters) {

    ListControlImplementation::InitializeParameters list_initialize_parameters;
    list_initialize_parameters.item_container = parameters.item_container;
    list_initialize_parameters.data_source =
        std::dynamic_pointer_cast<ListDataSource>(shared_from_this());
    list_initialize_parameters.delegate =
        std::dynamic_pointer_cast<ListControlDelegate>(shared_from_this());
    list_initialize_parameters.item_container_change_event = parameters.item_container_change_event;

    list_initialize_parameters.selection_change_event = std::bind(
        &TreeControlImplementation::OnListSelectionChange,
        this, std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3);

    list_implementation_->Initialize(list_initialize_parameters);
}


void TreeControlImplementation::InstallDataSource(
    const std::shared_ptr<TreeDataSource>& data_source) {

    ZAF_EXPECT(data_source);

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
    const std::shared_ptr<TreeControlDelegate>& delegate) {

    ZAF_EXPECT(delegate);

    delegate_ = delegate;
}


void TreeControlImplementation::SetDataSource(const std::shared_ptr<TreeDataSource>& data_source) {

    auto previous_data_source = data_source_.lock();
    InstallDataSource(data_source);

    if (data_source_change_event_) {
        data_source_change_event_(previous_data_source);
    }

    Reload();
}


void TreeControlImplementation::SetDelegate(const std::shared_ptr<TreeControlDelegate>& delegate) {

    auto previous_delegate = delegate_.lock();
    InstallDelegate(delegate);

    if (delegate_change_event_) {
        delegate_change_event_(previous_delegate);
    }

    Reload();
}


void TreeControlImplementation::Reload() {

    ReloadRootNode();

    list_implementation_->Reload();
}


void TreeControlImplementation::ReloadRootNode() {

    tree_index_mapping_.Clear();
    tree_data_manager_.Clear();
    expanded_data_set_.clear();
    selected_data_set_.clear();

    auto data_source = data_source_.lock();
    if (!data_source) {
        return;
    }

    auto child_count = data_source->GetChildDataCount(nullptr);

    tree_data_manager_.SetChildCount(nullptr, child_count);
    tree_index_mapping_.AddChildren({}, 0, child_count);
}


std::vector<std::shared_ptr<Object>> TreeControlImplementation::GetAllSelectedItems() const {

    std::vector<std::pair<std::shared_ptr<Object>, IndexPath>> selected_data_index_path;

    for (const auto& each_data : selected_data_set_) {

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


void TreeControlImplementation::SelectItem(const std::shared_ptr<Object>& data) {

    auto list_index = GetDataListIndex(data);
    if (list_index) {

        list_implementation_->SelectItemAtIndex(*list_index);
    }
    else {

        selected_data_set_.insert(data);
        NotifySelectionChange();
    }
}


void TreeControlImplementation::UnselectItem(const std::shared_ptr<Object>& data) {

    auto list_index = GetDataListIndex(data);
    if (list_index) {
        
        list_implementation_->UnselectItemAtIndex(*list_index);
    }
    else {

        auto erased_count = selected_data_set_.erase(data);
        if (erased_count > 0) {
            NotifySelectionChange();
        }
    }
}


void TreeControlImplementation::ExpandItem(const std::shared_ptr<Object>& data) {

    //Don't expand if it is already expanded.
    if (Contain(expanded_data_set_, data)) {
        return;
    }

    auto list_index = GetDataListIndex(data);
    if (list_index) {

        ExpandItemAtIndex(*list_index);
    }
    else {

        expanded_data_set_.insert(data);

        if (item_expand_event_) {
            item_expand_event_(data);
        }
    }
}


void TreeControlImplementation::CollapseItem(const std::shared_ptr<Object>& data) {

    //Don't collapse if it is already collapsed.
    if (!Contain(expanded_data_set_, data)) {
        return;
    }

    auto list_index = GetDataListIndex(data);
    if (list_index) {

        CollapseItemAtIndex(*list_index);
    }
    else {

        expanded_data_set_.erase(data);

        if (item_collapse_event_) {
            item_collapse_event_(data);
        }
    }
}


void TreeControlImplementation::ScrollToItem(const std::shared_ptr<Object>& data) {

    auto list_index = GetDataListIndex(data);
    if (!list_index) {
        return;
    }

    list_implementation_->ScrollToItemAtIndex(*list_index);
}


void TreeControlImplementation::ReloadItem(const std::shared_ptr<Object>& data) {

    auto list_index = GetDataListIndex(data);
    if (!list_index) {
        return;
    }

    NotifyDataUpdate(*list_index, 1);
}


std::size_t TreeControlImplementation::GetDataCount() {
    return tree_index_mapping_.GetNodeCount();
}


std::shared_ptr<Object> TreeControlImplementation::GetDataAtIndex(std::size_t index) {

    auto index_path = tree_index_mapping_.GetIndexPathAtIndex(index);
    auto tree_node = tree_data_manager_.GetNodeAtIndexPath(index_path);
    if (tree_node) {
        return tree_node->data;
    }

    std::shared_ptr<Object> parent_data;
    std::size_t child_index{};
    if (!GetParentDataAndChildIndex(index_path, parent_data, child_index)) {
        return Object::Empty();
    }

    auto data_source = data_source_.lock();
    if (!data_source) {
        return Object::Empty();
    }

    auto child_data = data_source->GetChildDataAtIndex(parent_data, child_index);

    tree_data_manager_.AddNode(parent_data, child_index, child_data);
    return child_data;
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


std::wstring TreeControlImplementation::GetItemText(
    std::size_t item_index,
    const std::shared_ptr<Object>& item_data) {

    std::shared_ptr<Object> parent_data;
    std::size_t child_index{};
    if (!GetParentDataAndChildIndex(item_index, parent_data, child_index)) {
        return {};
    }

    auto delegate = delegate_.lock();
    if (!delegate) {
        return {};
    }

    return delegate->GetItemText(parent_data, child_index, item_data);
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

    auto delegate = delegate_.lock();
    if (!delegate) {
        return nullptr;
    }

    auto tree_item = delegate->CreateItem(parent_data, child_index, item_data);
    tree_item->SetTreeControlImplementation(shared_from_this());
    return tree_item;
}


void TreeControlImplementation::LoadItem(
    const std::shared_ptr<ListItem>& item,
    std::size_t item_index,
    const std::shared_ptr<Object>& item_data) {

    auto index_path = tree_index_mapping_.GetIndexPathAtIndex(item_index);
    if (index_path.empty()) {
        return;
    }

    std::shared_ptr<Object> parent_data;
    std::size_t child_index{};
    if (!GetParentDataAndChildIndex(index_path, parent_data, child_index)) {
        return;
    }

    auto tree_item = std::dynamic_pointer_cast<TreeItem>(item);
    if (!tree_item) {
        return;
    }

    auto delegate = delegate_.lock();
    if (!delegate) {
        return;
    }

    delegate->LoadItem(tree_item, parent_data, child_index, item_data);

    tree_item->SetIndentLevel(index_path.size() - 1);
    SetItemExpandState(tree_item, item_data, index_path);
    SetItemSelectionState(tree_item, item_data);
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
        //Erase from expanded data set if the item cannot expand after updating.
        expanded_data_set_.erase(item_data);
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

    return Contain(expanded_data_set_, tree_node->data);
}


void TreeControlImplementation::SetItemSelectionState(
    const std::shared_ptr<TreeItem>& item, 
    const std::shared_ptr<Object>& item_data) {

    bool is_selected = Contain(selected_data_set_, item_data);
    item->SetIsSelected(is_selected);
}


void TreeControlImplementation::ExpandItemAtIndex(std::size_t list_item_index) {

    auto index_path = tree_index_mapping_.GetIndexPathAtIndex(list_item_index);
    if (index_path.empty()) {
        return;
    }

    std::shared_ptr<Object> expanded_data;
    std::size_t expanded_count{};
    if (!ExpandItemAtIndexPath(index_path, expanded_data, expanded_count)) {
        return;
    }

    expanded_data_set_.insert(expanded_data);

    NotifyDataAdd(list_item_index + 1, expanded_count);

    if (item_expand_event_) {
        item_expand_event_(expanded_data);
    }
}


bool TreeControlImplementation::ExpandItemAtIndexPath(
    const IndexPath& index_path,
    std::shared_ptr<Object>& expanded_data,
    std::size_t& expanded_count) {

    auto tree_node = tree_data_manager_.GetNodeAtIndexPath(index_path);
    if (tree_node && tree_node->children) {

        expanded_data = tree_node->data;
        expanded_count = ExpandItemWithTreeNode(*tree_node, index_path);
        return true;
    }

    return ExpandNewItem(index_path, expanded_data, expanded_count);
}


std::size_t TreeControlImplementation::ExpandItemWithTreeNode(
    const TreeNode& node, 
    const IndexPath& node_index_path) {

    if (!node.children) {
        return 0;
    }

    tree_index_mapping_.AddChildren(node_index_path, 0, node.children->size());
    std::size_t total_child_count = node.children->size();

    for (const auto& each_child_node : *node.children) {

        if (!each_child_node) {
            continue;
        }

        if (!Contain(expanded_data_set_, each_child_node->data)) {
            continue;
        }

        auto child_node_index_path = node_index_path;
        child_node_index_path.push_back(each_child_node->index_in_parent);

        total_child_count += ExpandItemWithTreeNode(*each_child_node, child_node_index_path);
    }

    return total_child_count;
}


bool TreeControlImplementation::ExpandNewItem(
    const IndexPath& index_path,
    std::shared_ptr<Object>& expanded_data,
    std::size_t& expanded_count) {

    std::shared_ptr<Object> parent_data;
    std::size_t index_in_parent{};
    if (!GetParentDataAndChildIndex(index_path, parent_data, index_in_parent)) {
        return false;
    }

    auto data_source = data_source_.lock();
    if (!data_source) {
        return false;
    }

    expanded_data = data_source->GetChildDataAtIndex(parent_data, index_in_parent);
    expanded_count = data_source->GetChildDataCount(expanded_data);

    tree_data_manager_.SetChildCount(expanded_data, expanded_count);
    tree_index_mapping_.AddChildren(index_path, 0, expanded_count);
    return true;
}


void TreeControlImplementation::CollapseItemAtIndex(std::size_t list_item_index) {

    auto index_path = tree_index_mapping_.GetIndexPathAtIndex(list_item_index);
    if (index_path.empty()) {
        return;
    }

    auto removed_count = tree_index_mapping_.RemoveAllChildrenRecursively(index_path);

    auto tree_node = tree_data_manager_.GetNodeAtIndexPath(index_path);
    if (tree_node) {
        expanded_data_set_.erase(tree_node->data);
    }

    NotifyDataRemove(list_item_index + 1, removed_count);

    if (item_collapse_event_) {
        item_collapse_event_(tree_node->data);
    }
}


void TreeControlImplementation::OnItemExpandChange(
    const std::shared_ptr<TreeItem>& item, 
    bool is_expanded) {

    auto list_item_index = list_implementation_->GetListItemIndex(item);
    if (list_item_index == InvalidIndex) {
        return;
    }

    if (is_expanded) {
        ExpandItemAtIndex(list_item_index);
    }
    else {
        CollapseItemAtIndex(list_item_index);
    }
}


void TreeControlImplementation::OnListSelectionChange(
    ListSelectionChangeReason reason,
    std::size_t index,
    std::size_t count) {

    switch (reason) {
    case ListSelectionChangeReason::AddSelection:
        ModifySelection(index, count, false);
        break;
    case ListSelectionChangeReason::ReplaceSelection:
        ModifySelection(index, count, true);
        break;
    case ListSelectionChangeReason::RemoveSelection:
        RemoveSelection(index, count);
        break;
    case ListSelectionChangeReason::ItemChange:
    default:
        return;
    }

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
        selected_data_set_.clear();
    }

    for (auto list_index : zaf::Range(index, index + count)) {

        auto index_path = tree_index_mapping_.GetIndexPathAtIndex(list_index);
        if (index_path.empty()) {
            continue;
        }

        auto node = tree_data_manager_.GetNodeAtIndexPath(index_path);
        if (!node) {
            continue;
        }

        selected_data_set_.insert(node->data);
    }
}


void TreeControlImplementation::RemoveSelection(std::size_t index, std::size_t count) {

    for (auto list_index : zaf::Range(index, index + count)) {

        auto index_path = tree_index_mapping_.GetIndexPathAtIndex(list_index);
        if (index_path.empty()) {
            continue;
        }

        auto node = tree_data_manager_.GetNodeAtIndexPath(index_path);
        if (!node) {
            continue;
        }

        selected_data_set_.erase(node->data);
    }
}


void TreeControlImplementation::OnDataAdd(const TreeDataSourceDataAddInfo& event_info) {

    //Get parent index path.
    auto parent_index_path = tree_data_manager_.GetIndexPathOfData(event_info.parent_data);
    if (!parent_index_path) {
        return;
    }

    tree_data_manager_.AddChildren(event_info.parent_data, event_info.index, event_info.count);

    //Update parent item.
    auto parent_list_index = tree_index_mapping_.GetIndexAtIndexPath(*parent_index_path);
    if (parent_list_index != InvalidIndex) {
        NotifyDataUpdate(parent_list_index, 1);
    }

    //Get list index at which to insert new items.
    auto list_index = GetChildListIndex(*parent_index_path, event_info.index);
    if (list_index == InvalidIndex) {
        return;
    }

    //Add children to tree ui data.
    tree_index_mapping_.AddChildren(*parent_index_path, event_info.index, event_info.count);

    //Raise event.
    NotifyDataAdd(list_index, event_info.count);
}


std::size_t TreeControlImplementation::GetChildListIndex(
    const IndexPath& parent_index_path,
    std::size_t child_index) const {

    auto old_child_count = tree_index_mapping_.GetChildCount(parent_index_path);
    if (!old_child_count) {
        return InvalidIndex;
    }

    //Child index must not exceed child count.
    ZAF_EXPECT(child_index <= old_child_count);

    //There is no child, get next index of parent's index.
    if (old_child_count == 0) {

        auto list_index = tree_index_mapping_.GetIndexAtIndexPath(parent_index_path);
        if (list_index != InvalidIndex) {
            list_index += 1;
        }
        return list_index;
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
    if (list_index != InvalidIndex) {

        //Add child count recursively.
        auto recursive_child_count = tree_index_mapping_.GetChildCountRecursively(child_index_path);
        if (recursive_child_count) {
            list_index += *recursive_child_count;
        }

        list_index += 1;
    }
    return list_index;
}


void TreeControlImplementation::OnDataRemove(const TreeDataSourceDataRemoveInfo& event_info) {

    //Get parent index path.
    auto parent_index_path = tree_data_manager_.GetIndexPathOfData(event_info.parent_data);
    if (!parent_index_path) {
        return;
    }

    //Remove tree nodes.
    auto removed_data_list = tree_data_manager_.RemoveChildren(
        event_info.parent_data,
        event_info.index,
        event_info.count);

    RemoveDataFromSets(removed_data_list);

    //Update parent item.
    auto parent_list_index = tree_index_mapping_.GetIndexAtIndexPath(*parent_index_path);
    if (parent_list_index != InvalidIndex) {
        NotifyDataUpdate(parent_list_index, 1);
    }

    //Get list index at which begin removing.
    auto child_index_path = *parent_index_path;
    child_index_path.push_back(event_info.index);

    auto list_index = tree_index_mapping_.GetIndexAtIndexPath(child_index_path);
    if (list_index == InvalidIndex) {
        return;
    }

    //Remove from ui data.
    auto total_remove_count = tree_index_mapping_.RemoveChildren(
        *parent_index_path,
        event_info.index,
        event_info.count);

    //Raise event.
    NotifyDataRemove(list_index, total_remove_count);
}


void TreeControlImplementation::RemoveDataFromSets(
    const std::vector<std::shared_ptr<Object>>& removed_data_list) {

    for (const auto& each_data : removed_data_list) {
        if (selected_data_set_.empty()) {
            break;
        }
        selected_data_set_.erase(each_data);
    }

    for (const auto& each_data : removed_data_list) {
        if (expanded_data_set_.empty()) {
            break;
        }
        expanded_data_set_.erase(each_data);
    }
}


void TreeControlImplementation::OnDataUpdate(const TreeDataSourceDataUpdateInfo& event_info) {

    auto updated_list_indexes = GetChildrenListIndexes(
        event_info.parent_data, 
        event_info.index,
        event_info.count);

    if (updated_list_indexes.empty()) {
        return;
    }

    //Merge adjacent indexes.
    std::vector<std::pair<std::size_t, std::size_t>> updated_index_ranges;

    std::size_t range_index = updated_list_indexes.front();
    std::size_t range_length = 1;
    std::size_t last_index = range_index;

    for (auto i : zaf::Range(1u, updated_list_indexes.size())) {

        auto current_index = updated_list_indexes[i];

        if (last_index + 1 == current_index) {
            range_length++;
        }
        else {

            updated_index_ranges.push_back(std::make_pair(range_index, range_length));

            range_index = current_index;
            range_length = 1;
        }

        last_index = current_index;
    }

    updated_index_ranges.push_back(std::make_pair(range_index, range_length));

    //Raise events.
    for (const auto& each_range : updated_index_ranges) {
        NotifyDataUpdate(each_range.first, each_range.second);
    }
}


std::vector<std::size_t> TreeControlImplementation::GetChildrenListIndexes(
    const std::shared_ptr<Object>& parent_data,
    std::size_t index,
    std::size_t count) {

    auto index_path = tree_data_manager_.GetIndexPathOfData(parent_data);
    if (!index_path) {
        return {};
    }

    std::vector<std::size_t> result;

    IndexPath child_index_path = *index_path;
    for (auto index : zaf::Range(index, index + count)) {

        child_index_path.push_back(index);

        auto list_index = tree_index_mapping_.GetIndexAtIndexPath(child_index_path);
        if (list_index != InvalidIndex) {

            result.push_back(list_index);
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

    auto list_index = tree_index_mapping_.GetIndexAtIndexPath(*index_path);
    if (list_index == InvalidIndex) {
        return std::nullopt;
    }

    return list_index;
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