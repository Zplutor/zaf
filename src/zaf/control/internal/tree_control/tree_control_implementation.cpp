#include <zaf/control/internal/tree_control/tree_control_implementation.h>
#include <zaf/base/container/utility/contain.h>
#include <zaf/base/container/utility/find.h>
#include <zaf/base/container/utility/range.h>
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

    selection_change_event_ = parameters.selection_change_event;

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

    data_source_ = data_source;
}


void TreeControlImplementation::InstallDelegate(
    const std::shared_ptr<TreeControlDelegate>& delegate) {

    ZAF_EXPECT(delegate);

    delegate_ = delegate;
}


void TreeControlImplementation::SetDataSource(const std::shared_ptr<TreeDataSource>& data_source) {

    InstallDataSource(data_source);

    Reload();
}


void TreeControlImplementation::SetDelegate(const std::shared_ptr<TreeControlDelegate>& delegate) {

    InstallDelegate(delegate);

    Reload();
}


void TreeControlImplementation::Reload() {

    ReloadRootNode();

    list_implementation_->Reload();
}


void TreeControlImplementation::ReloadRootNode() {

    tree_data_ = TreeData{};
    expand_manager_.Clear();
    selection_manager_.Clear();

    auto data_source = data_source_.lock();
    if (!data_source) {
        return;
    }

    ExpandNodeInfo root_node_info;
    root_node_info.children_count = data_source->GetChildDataCount(nullptr);
    expand_manager_.Expand(IndexPath{}, root_node_info, tree_data_);
}


std::vector<std::shared_ptr<Object>> TreeControlImplementation::GetAllSelectedItemData() const {
    return selection_manager_.GetAllSelectedNodeData();
}


std::shared_ptr<Object> TreeControlImplementation::GetFirstSelectedItemData() const {
    return selection_manager_.GetFirstSelectedNodeData();
}


std::size_t TreeControlImplementation::GetDataCount() {
    return tree_data_.GetNodeCount();
}


std::shared_ptr<Object> TreeControlImplementation::GetDataAtIndex(std::size_t index) {

    auto index_path = tree_data_.GetIndexPathAtIndex(index);
    auto node_info = expand_manager_.GetNodeInfo(index_path);
    if (node_info) {
        return node_info->node_data;
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

    return data_source->GetChildDataAtIndex(parent_data, child_index);
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

    auto index_path = tree_data_.GetIndexPathAtIndex(item_index);
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

    tree_item->SetIndentDeep(index_path.size() - 1);
    SetItemExpandState(tree_item, item_data, index_path);
    SetItemSelectionState(tree_item, index_path);
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
    else if (expand_manager_.IsNodeExpanded(index_path)) {
        item->SetExpandState(ExpandState::Expanded);
    }
    else {
        item->SetExpandState(ExpandState::Collapsed);
    }
}


void TreeControlImplementation::SetItemSelectionState(
    const std::shared_ptr<TreeItem>& item, 
    const IndexPath& index_path) {

    bool is_selected = selection_manager_.IsIndexPathSelected(index_path);
    item->SetIsSelected(is_selected);
}


void TreeControlImplementation::ExpandItem(std::size_t list_item_index) {

    auto index_path = tree_data_.GetIndexPathAtIndex(list_item_index);
    if (index_path.empty()) {
        return;
    }

    std::shared_ptr<Object> parent_data;
    std::size_t index_in_parent{};
    if (!GetParentDataAndChildIndex(index_path, parent_data, index_in_parent)) {
        return;
    }

    auto data_source = data_source_.lock();
    if (!data_source) {
        return;
    }

    auto data = data_source->GetChildDataAtIndex(parent_data, index_in_parent);
    std::size_t children_count = data_source->GetChildDataCount(data);

    ExpandNodeInfo node_info;
    node_info.node_data = data;
    node_info.children_count = children_count;

    auto expanded_children_count = expand_manager_.Expand(index_path, node_info, tree_data_);

    NotifyDataUpdate(list_item_index, 1);
    NotifyDataAdd(list_item_index + 1, expanded_children_count);
}


void TreeControlImplementation::CollapseItem(std::size_t list_item_index) {

    auto index_path = tree_data_.GetIndexPathAtIndex(list_item_index);
    if (index_path.empty()) {
        return;
    }

    auto removed_count = tree_data_.RemoveAllChildrenRecursively(index_path);
    expand_manager_.Collapse(index_path);

    NotifyDataUpdate(list_item_index, 1);
    NotifyDataRemove(list_item_index + 1, removed_count);
}


bool TreeControlImplementation::GetParentDataAndChildIndex(
    std::size_t list_index,
    std::shared_ptr<Object>& parent_data,
    std::size_t& child_index) {

    auto index_path = tree_data_.GetIndexPathAtIndex(list_index);
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

    auto parent_node_info = expand_manager_.GetNodeInfo(parent_index_path);
    if (!parent_node_info) {
        return false;
    }

    parent_data = parent_node_info->node_data;
    return true;
}


void TreeControlImplementation::OnItemExpandChange(
    const std::shared_ptr<TreeItem>& item, 
    bool is_expanded) {

    auto list_item_index = list_implementation_->GetListItemIndex(item);
    if (list_item_index == InvalidIndex) {
        return;
    }

    if (is_expanded) {
        ExpandItem(list_item_index);
    }
    else {
        CollapseItem(list_item_index);
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

    if (selection_change_event_) {
        selection_change_event_();
    }
}


void TreeControlImplementation::ModifySelection(
    std::size_t index, 
    std::size_t count, 
    bool is_replace) {

    auto data_source = data_source_.lock();
    if (!data_source) {
        return;
    }

    std::map<IndexPath, std::shared_ptr<Object>> affected_nodes;
    for (auto list_index : zaf::Range(index, index + count)) {

        auto index_path = tree_data_.GetIndexPathAtIndex(list_index);
        if (index_path.empty()) {
            continue;
        }

        std::shared_ptr<Object> parent_data;
        std::size_t child_index{};
        if (!GetParentDataAndChildIndex(index_path, parent_data, child_index)) {
            continue;
        }

        auto data = data_source->GetChildDataAtIndex(parent_data, child_index);
        affected_nodes[index_path] = data;
    }

    if (is_replace) {
        selection_manager_.Clear();
    }

    selection_manager_.AddSelectedNodes(affected_nodes);
}


void TreeControlImplementation::RemoveSelection(std::size_t index, std::size_t count) {

    std::vector<IndexPath> affected_index_paths;
    for (auto list_index : zaf::Range(index, index + count)) {

        auto index_path = tree_data_.GetIndexPathAtIndex(list_index);
        affected_index_paths.push_back(index_path);
    }

    selection_manager_.RemoveSelectedNodes(affected_index_paths);
}

}