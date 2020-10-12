#include <zaf/control/internal/tree_control/tree_control_implementation.h>
#include <zaf/base/container/utility/contain.h>
#include <zaf/base/container/utility/find.h>
#include <zaf/base/error/check.h>

namespace zaf::internal {

TreeControlImplementation::TreeControlImplementation(ScrollableControl& owner) :
    list_implementation_(std::make_shared<internal::ListControlImplementation>(owner)) {

}


void TreeControlImplementation::Initialize(const InitializeParameters& parameters) {

    InstallDataSource(parameters.data_source);
    InstallDelegate(parameters.delegate);

    InitializeListImplementation(parameters);

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

    expanded_node_data_.clear();
    expanded_node_data_[zaf::IndexPath{}] = nullptr;

    tree_data_ = TreeData{};

    auto data_source = data_source_.lock();
    if (!data_source) {
        return;
    }

    std::size_t child_count = data_source->GetChildDataCount(nullptr);
    tree_data_.AddChildren({}, 0, child_count);
}


std::size_t TreeControlImplementation::GetDataCount() {
    return tree_data_.GetNodeCount();
}


std::shared_ptr<Object> TreeControlImplementation::GetDataAtIndex(std::size_t index) {

    auto index_path = tree_data_.GetIndexPathAtIndex(index);
    auto data = Find(expanded_node_data_, index_path);
    if (data) {
        return *data;
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
    else if (Contain(expanded_node_data_, index_path)) {
        item->SetExpandState(ExpandState::Expanded);
    }
    else {
        item->SetExpandState(ExpandState::Collapsed);
    }
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
    std::size_t child_count = data_source->GetChildDataCount(data);

    tree_data_.AddChildren(index_path, 0, child_count);
    expanded_node_data_[index_path] = data;

    NotifyDataUpdate(list_item_index, 1);
    NotifyDataAdd(list_item_index + 1, child_count);
}


void TreeControlImplementation::CollapseItem(std::size_t list_item_index) {

    auto index_path = tree_data_.GetIndexPathAtIndex(list_item_index);
    if (index_path.empty()) {
        return;
    }

    auto removed_count = tree_data_.RemoveAllChildrenRecursively(index_path);
    expanded_node_data_.erase(index_path);

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

    auto parent_data_pointer = Find(expanded_node_data_, parent_index_path);
    if (!parent_data_pointer) {
        return false;
    }

    parent_data = *parent_data_pointer;
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

}