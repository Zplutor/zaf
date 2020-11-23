#include <zaf/control/internal/tree_control/tree_data_manager.h>
#include <zaf/base/container/utility/find.h>
#include <zaf/base/error/check.h>

namespace zaf::internal {

TreeDataManager::TreeDataManager() {

    AddRootNodeToMap();
}


std::shared_ptr<const TreeNode> TreeDataManager::GetNodeAtIndexPath(
    const IndexPath& index_path) const {

    auto current_node = root_node_;

    for (auto each_index : index_path) {

        auto iterator = std::lower_bound(
            current_node->children.begin(),
            current_node->children.end(),
            each_index,
            [](const auto& node, std::size_t index) {
                return node->index_in_parent < index;
            }
        );

        if (iterator == current_node->children.end() ||
            (*iterator)->index_in_parent != each_index) {
            return nullptr;
        }

        current_node = *iterator;
    }

    return current_node;
}


std::optional<IndexPath> TreeDataManager::GetIndexPathOfData(const std::shared_ptr<Object>& data) {

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


void TreeDataManager::AddChildren(
    const std::shared_ptr<Object>& parent_data,
    std::size_t parent_index,
    std::size_t count) {

    if (count <= 0) {
        return;
    }

    auto parent_node = Find(data_map_, parent_data);
    if (!parent_node) {
        return;
    }

    (*parent_node)->child_count += count;

    for (const auto& each_child : (*parent_node)->children) {

        if (each_child->index_in_parent >= parent_index) {
            each_child->index_in_parent += count;
        }
    }
}


void TreeDataManager::AddNewNode(
    const std::shared_ptr<Object>& parent_data,
    std::size_t parent_index,
    const std::shared_ptr<Object>& data,
    std::size_t child_count) {

    auto parent_node = Find(data_map_, parent_data);
    if (!parent_node) {
        return;
    }

    ZAF_CHECK(parent_index <= (*parent_node)->child_count);

    auto new_node = std::make_shared<TreeNode>();
    new_node->parent = *parent_node;
    new_node->index_in_parent = parent_index;
    new_node->data = data;
    new_node->child_count = child_count;
    
    auto& parent_children = (*parent_node)->children;
    auto insert_position = std::lower_bound(
        parent_children.begin(),
        parent_children.end(), 
        parent_index, 
        [](const auto& node, std::size_t index) {
            return node->index_in_parent < index;
        }
    );

    ZAF_CHECK(
        insert_position == parent_children.end() || 
        (*insert_position)->index_in_parent != parent_index);

    parent_children.insert(insert_position, new_node);

    data_map_[data] = new_node;
}


void TreeDataManager::RemoveChildren(
    const std::shared_ptr<Object>& parent_data,
    std::size_t parent_index,
    std::size_t count) {

    auto parent_node = Find(data_map_, parent_data);
    if (!parent_node) {
        return;
    }

    (*parent_node)->child_count -= count;

    auto& parent_children = (*parent_node)->children;

    auto iterator = parent_children.begin();
    while (iterator != parent_children.end()) {

        const auto& each_child = *iterator;

        ZAF_CHECK(each_child->index_in_parent != InvalidIndex);

        if (each_child->index_in_parent < parent_index) {
            iterator++;
            continue;
        }

        if (each_child->index_in_parent >= parent_index + count) {
            each_child->index_in_parent -= count;
            iterator++;
            continue;
        }
        
        RemoveDataFromMapRecursively(each_child->data);

        iterator = parent_children.erase(iterator);
    }
}


void TreeDataManager::RemoveDataFromMapRecursively(const std::shared_ptr<Object>& data) {

    auto iterator = data_map_.find(data);
    if (iterator == data_map_.end()) {
        return;
    }

    for (const auto& each_child : iterator->second->children) {
        RemoveDataFromMapRecursively(each_child->data);
    }

    data_map_.erase(iterator);
}


void TreeDataManager::Clear() {

    root_node_ = std::make_shared<TreeNode>();
    data_map_.clear();

    AddRootNodeToMap();
}


void TreeDataManager::AddRootNodeToMap() {

    data_map_[nullptr] = root_node_;
}

}