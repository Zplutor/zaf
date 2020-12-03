#include <zaf/control/internal/tree_control/tree_data_manager.h>
#include <zaf/base/container/utility/find.h>
#include <zaf/base/container/utility/range.h>
#include <zaf/base/error/check.h>

namespace zaf::internal {

TreeDataManager::TreeDataManager() {

    Initialize();
}


void TreeDataManager::Initialize() {

    root_node_ = std::make_shared<TreeNode>();
    data_map_[nullptr] = root_node_;
}


std::shared_ptr<const TreeNode> TreeDataManager::GetNodeAtIndexPath(
    const IndexPath& index_path) const {

    auto current_node = root_node_;

    for (auto each_index : index_path) {

        if (!current_node->children) {
            return nullptr;
        }

        if (each_index >= current_node->children->size()) {
            return nullptr;
        }

        current_node = (*current_node->children)[each_index];
    }

    return current_node;
}


std::optional<IndexPath> TreeDataManager::GetIndexPathOfData(
    const std::shared_ptr<Object>& data) const {

    auto node = Find(data_map_, data);
    if (!node) {
        return std::nullopt;
    }

    IndexPath result;

    auto current_node = *node;
    while (current_node) {

        if (current_node->index_in_parent != InvalidIndex) {
            result.push_back(current_node->index_in_parent);
        }

        current_node = current_node->parent.lock();
    }

    std::reverse(result.begin(), result.end());
    return result;
}


void TreeDataManager::AddNode(
    const std::shared_ptr<Object>& parent_data,
    std::size_t index_in_parent,
    const std::shared_ptr<Object>& data) {

    auto parent_node = Find(data_map_, parent_data);
    if (!parent_node) {
        return;
    }

    auto& parent_children = (*parent_node)->children;

    ZAF_EXPECT(parent_children);
    ZAF_EXPECT(index_in_parent < parent_children->size());
    ZAF_EXPECT((*parent_children)[index_in_parent] == nullptr);

    auto new_node = std::make_shared<TreeNode>();
    new_node->parent = *parent_node;
    new_node->index_in_parent = index_in_parent;
    new_node->data = data;

    (*parent_children)[index_in_parent] = new_node;
    data_map_[data] = new_node;
}


void TreeDataManager::SetChildCount(const std::shared_ptr<Object>& data, std::size_t child_count) {

    auto node = Find(data_map_, data);
    if (!node) {
        return;
    }

    auto& node_children = (*node)->children;
    ZAF_EXPECT(!node_children);

    node_children = std::vector<std::shared_ptr<TreeNode>>{};
    node_children->resize(child_count);
}


void TreeDataManager::AddChildren(
    const std::shared_ptr<Object>& parent_data,
    std::size_t child_index,
    std::size_t count) {

    if (count <= 0) {
        return;
    }

    auto parent_node = Find(data_map_, parent_data);
    if (!parent_node) {
        return;
    }

    auto& parent_children = (*parent_node)->children;
    if (!parent_children) {
        return;
    }

    ZAF_EXPECT(child_index <= parent_children->size());

    for (auto index : zaf::Range(child_index, parent_children->size())) {

        const auto& child_node = (*parent_children)[index];
        if (child_node) {
            child_node->index_in_parent += count;
        }
    }

    parent_children->insert(parent_children->begin() + child_index, count, nullptr);
}


std::vector<std::shared_ptr<Object>> TreeDataManager::RemoveChildren(
    const std::shared_ptr<Object>& parent_data,
    std::size_t child_index,
    std::size_t count) {

    auto parent_node = Find(data_map_, parent_data);
    if (!parent_node) {
        return {};
    }

    auto& parent_children = (*parent_node)->children;
    if (!parent_children) {
        return {};
    }

    ZAF_EXPECT(
        (child_index < parent_children->size()) && 
        (child_index + count <= parent_children->size()));

    std::vector<std::shared_ptr<Object>> removed_data_list;

    //Remove inner children recursively.
    auto begin_iterator = std::next(parent_children->begin(), child_index);
    auto end_iterator = std::next(begin_iterator, count);

    for (auto current_iterator = begin_iterator; 
         current_iterator != end_iterator; 
        ++current_iterator) {

        const auto& current_child = *current_iterator;
        if (current_child) {
            RemoveDataFromMapRecursively(current_child->data, removed_data_list);
        }
    }

    //Revise indexes in parent.
    for (auto current_iterator = end_iterator; 
         current_iterator != parent_children->end(); 
         ++current_iterator) {

        const auto& current_child = *current_iterator;
        if (current_child) {
            current_child->index_in_parent -= count;
        }
    }

    //Remove children.
    parent_children->erase(begin_iterator, end_iterator);

    if (parent_children->empty()) {
        parent_children.reset();
    }

    return removed_data_list;
}


void TreeDataManager::RemoveDataFromMapRecursively(
    const std::shared_ptr<Object>& data,
    std::vector<std::shared_ptr<Object>>& removed_data_list) {

    auto iterator = data_map_.find(data);
    if (iterator == data_map_.end()) {
        return;
    }

    if (iterator->second->children) {
        for (const auto& each_child : *iterator->second->children) {
            RemoveDataFromMapRecursively(each_child->data, removed_data_list);
        }
    }

    data_map_.erase(iterator);
    removed_data_list.push_back(data);
}


void TreeDataManager::Clear() {

    data_map_.clear();
    Initialize();
}

}