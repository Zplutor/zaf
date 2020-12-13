#include <zaf/control/internal/tree_control/tree_data_manager.h>
#include <zaf/base/container/utility/contain.h>
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

        if (!current_node->is_expanded) {
            return nullptr;
        }

        if (each_index >= current_node->children.size()) {
            return nullptr;
        }

        current_node = current_node->children[each_index];
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


bool TreeDataManager::IsNodeExpanded(const std::shared_ptr<Object>& data) const {

    auto node = Find(data_map_, data);
    if (!node) {
        return false;
    }

    return (*node)->is_expanded;
}


void TreeDataManager::ExpandNode(const std::shared_ptr<Object>& data, std::size_t child_count) {

    auto node_pointer = Find(data_map_, data);
    if (!node_pointer) {
        return;
    }

    const auto& node = *node_pointer;
    ZAF_EXPECT(!node->is_expanded);

    std::vector<std::shared_ptr<TreeNode>> new_children;
    new_children.resize(child_count);

    for (const auto& each_child : node->children) {

        ZAF_EXPECT(each_child->index_in_parent < new_children.size());
        new_children[each_child->index_in_parent] = each_child;
    }
    
    std::swap(node->children, new_children);
    node->is_expanded = true;
}


void TreeDataManager::SetNode(
    const std::shared_ptr<Object>& parent_data,
    std::size_t index_in_parent, 
    const std::shared_ptr<Object>& data) {

    auto parent_node_pointer = Find(data_map_, parent_data);
    if (!parent_node_pointer) {
        return;
    }

    const auto& parent_node = *parent_node_pointer;
    ZAF_EXPECT(parent_node->is_expanded);
    ZAF_EXPECT(index_in_parent < parent_node->children.size());
    ZAF_EXPECT(parent_node->children[index_in_parent] == nullptr);

    auto new_node = std::make_shared<TreeNode>();
    new_node->parent = parent_node;
    new_node->index_in_parent = index_in_parent;
    new_node->data = data;

    parent_node->children[index_in_parent] = new_node;
    data_map_[data] = new_node;
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

    if ((*parent_node)->is_expanded) {
        AddChildrenToExpandedNode(**parent_node, child_index, count);
    }
    else {
        AddChildrenToCollapsedNode(**parent_node, child_index, count);
    }
}


void TreeDataManager::AddChildrenToExpandedNode(
    TreeNode& parent_node,
    std::size_t child_index,
    std::size_t count) {

    auto& parent_children = parent_node.children;
    ZAF_EXPECT(child_index <= parent_children.size());

    for (auto index : zaf::Range(child_index, parent_children.size())) {

        const auto& child_node = parent_children[index];
        if (child_node) {
            child_node->index_in_parent += count;
        }
    }

    parent_children.insert(parent_children.begin() + child_index, count, nullptr);
}


void TreeDataManager::AddChildrenToCollapsedNode(
    TreeNode& parent_node,
    std::size_t child_index,
    std::size_t count) {

    for (const auto& each_child : parent_node.children) {

        ZAF_EXPECT(each_child);

        if (each_child->index_in_parent >= child_index) {
            each_child->index_in_parent += count;
        }
    }
}


TreeNodeOperationResult TreeDataManager::RemoveChildren(
    const std::shared_ptr<Object>& parent_data,
    std::size_t child_index,
    std::size_t count) {

    auto parent_node = Find(data_map_, parent_data);
    if (!parent_node) {
        return {};
    }

    TreeNodeOperationResult result;

    if ((*parent_node)->is_expanded) {
        RemoveChildrenFromExpandedNode(
            **parent_node, 
            child_index, 
            count, 
            result.selection_changed);
    }
    else {
        RemoveChildrenFromCollapsedNode(
            **parent_node, 
            child_index, 
            count, 
            result.selection_changed);
    }

    return result;
}


void TreeDataManager::RemoveChildrenFromExpandedNode(
    TreeNode& parent_node,
    std::size_t child_index,
    std::size_t count,
    bool& selection_changed) {

    //Remove inner children recursively.
    auto begin_iterator = std::next(parent_node.children.begin(), child_index);
    auto end_iterator = std::next(begin_iterator, count);

    for (auto current_iterator = begin_iterator;
         current_iterator != end_iterator;
         ++current_iterator) {

        const auto& current_child = *current_iterator;
        if (current_child) {
            RemoveDataFromMapRecursively(current_child->data, selection_changed);
        }
    }

    //Revise indexes in parent.
    for (auto current_iterator = end_iterator;
         current_iterator != parent_node.children.end();
         ++current_iterator) {

        const auto& current_child = *current_iterator;
        if (current_child) {
            current_child->index_in_parent -= count;
        }
    }

    //Remove children.
    parent_node.children.erase(begin_iterator, end_iterator);
}


void TreeDataManager::RemoveChildrenFromCollapsedNode(
    TreeNode& parent_node,
    std::size_t child_index,
    std::size_t count,
    bool& selection_changed) {

    auto iterator = parent_node.children.begin();
    while (iterator != parent_node.children.end()) {

        const auto& child = *iterator;
        if (child_index <= child->index_in_parent && 
            child->index_in_parent < child_index + count) {

            RemoveDataFromMapRecursively(child->data, selection_changed);

            iterator = parent_node.children.erase(iterator);
        }
        else {
            ++iterator;
        }
    }
}


void TreeDataManager::RemoveDataFromMapRecursively(
    const std::shared_ptr<Object>& data,
    bool& selection_changed) {

    auto iterator = data_map_.find(data);
    if (iterator == data_map_.end()) {
        return;
    }

    for (const auto& each_child : iterator->second->children) {
        RemoveDataFromMapRecursively(each_child->data, selection_changed);
    }
    
    data_map_.erase(iterator);

    if (selected_data_set_.erase(data) > 0) {
        selection_changed = true;
    }
}


TreeNodeOperationResult TreeDataManager::CollapseNode(const std::shared_ptr<Object>& data) {

    auto node = Find(data_map_, data);
    if (!node) {
        return {};
    }

    TreeNodeOperationResult result;
    CollapseNodeRecursively(**node, result.selection_changed);
    return result;
}


void TreeDataManager::CollapseNodeRecursively(TreeNode& node, bool& selection_changed) {

    ZAF_EXPECT(node.is_expanded);

    std::vector<std::shared_ptr<TreeNode>> new_children;

    for (const auto& each_child : node.children) {

        if (!each_child) {
            continue;
        }

        if (selected_data_set_.erase(each_child->data) > 0) {
            selection_changed = true;
        }

        if (each_child->is_expanded) {
            new_children.push_back(each_child);
        }
        else {
            data_map_.erase(each_child->data);
        }
    }

    std::swap(node.children, new_children);

    //Recursive
    for (const auto& each_child : node.children) {
        CollapseNodeRecursively(*each_child, selection_changed);
    }

    node.is_expanded = false;
}


TreeNodeOperationResult TreeDataManager::CollapseNodeWithoutReserveExpandState(
    const std::shared_ptr<Object>& data) {

    auto node = Find(data_map_, data);
    if (!node) {
        return {};
    }

    TreeNodeOperationResult result;
    CollapseNodeWithoutReserveExpandStateRecursively(**node, result.selection_changed);
    return result;
}


void TreeDataManager::CollapseNodeWithoutReserveExpandStateRecursively(
    TreeNode& node, 
    bool& selection_changed) {

    for (const auto& each_child : node.children) {

        if (!each_child) {
            continue;
        }

        if (selected_data_set_.erase(each_child->data) > 0) {
            selection_changed = true;
        }

        data_map_.erase(each_child->data);

        //Recursively
        CollapseNodeRecursively(*each_child, selection_changed);
    }

    node.children.clear();
    node.is_expanded = false;
}


void TreeDataManager::SelectNode(const std::shared_ptr<Object>& data) {
    selected_data_set_.insert(data);
}


void TreeDataManager::UnselectNode(const std::shared_ptr<Object>& data) {
    selected_data_set_.erase(data);
}


void TreeDataManager::UnselectAllNodes() {
    selected_data_set_.clear();
}


bool TreeDataManager::IsNodeSelected(const std::shared_ptr<Object>& data) const {
    return Contain(selected_data_set_, data);
}


const TreeDataManager::TreeDataSet& TreeDataManager::GetAllSelectedNodes() const {
    return selected_data_set_;
}


void TreeDataManager::Clear() {

    data_map_.clear();
    selected_data_set_.clear();
    Initialize();
}

}