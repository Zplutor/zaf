#pragma once

#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <zaf/base/define.h>
#include <zaf/control/internal/tree_control/tree_data_helpers.h>
#include <zaf/control/internal/tree_control/tree_node.h>
#include <zaf/control/internal/tree_control/tree_node_children_adder.h>
#include <zaf/control/internal/tree_control/tree_node_expander.h>

namespace zaf::internal {

class TreeNodeOperationResult {
public:
    bool selection_changed{};
};


class TreeDataManager {
public:
    using TreeDataSet = std::unordered_set<std::shared_ptr<Object>, TreeDataHash, TreeDataEqual>;

public:
    TreeDataManager();

    TreeDataManager(const TreeDataManager&) = delete;
    TreeDataManager& operator=(const TreeDataManager&) = delete;

    std::shared_ptr<const TreeNode> GetNodeAtIndexPath(const IndexPath& index_path) const;
    std::optional<IndexPath> GetIndexPathOfData(const std::shared_ptr<Object>& data) const;

    bool IsNodeExpanded(const std::shared_ptr<Object>& data) const;

    std::shared_ptr<TreeNodeExpander> ExpandNodeAtIndexPath(const IndexPath& index_path);

    std::shared_ptr<TreeNodeChildrenAdder> AddChildrenAtIndexPath(const IndexPath& index_path);

    TreeNodeOperationResult RemoveChildren(
        const std::shared_ptr<Object>& parent_data,
        std::size_t child_index, 
        std::size_t count);

    TreeNodeOperationResult CollapseNode(const std::shared_ptr<Object>& data);
    TreeNodeOperationResult CollapseNodeWithoutReserveExpandState(
        const std::shared_ptr<Object>& data);

    void SelectNode(const std::shared_ptr<Object>& data);
    void UnselectNode(const std::shared_ptr<Object>& data);
    void UnselectAllNodes();
    bool IsNodeSelected(const std::shared_ptr<Object>& data) const;
    const TreeDataSet& GetAllSelectedNodes() const;

    void Clear();

private:
    friend class TreeNodeChildrenAdder;
    friend class TreeNodeExpander;

    void ExpandNodeWithChildCount(TreeNode& node, std::size_t child_count);
    void SetChildDataToNode(
        const std::shared_ptr<TreeNode>& parent_node, 
        std::size_t index_in_parent, 
        const std::shared_ptr<Object>& data);
    void AddChildrenToNode(
        const std::shared_ptr<TreeNode>& parent_node, 
        std::size_t index, 
        std::size_t count);

private:
    using TreeDataMap = std::unordered_map<
        std::shared_ptr<Object>,
        std::shared_ptr<TreeNode>,
        TreeDataHash,
        TreeDataEqual
    >;

private:
    void Initialize();

    std::shared_ptr<TreeNode> InnerGetNodeAtIndexPath(const IndexPath& index_path) const;

    void AddChildrenToExpandedNode(
        TreeNode& parent_node,
        std::size_t child_index,
        std::size_t count);
    void AddChildrenToCollapsedNode(
        TreeNode& parent_node,
        std::size_t child_index,
        std::size_t count);

    void RemoveChildrenFromExpandedNode(
        TreeNode& parent_node, 
        std::size_t child_index, 
        std::size_t count,
        bool& selection_changed);
    void RemoveChildrenFromCollapsedNode(
        TreeNode& parent_node,
        std::size_t child_index,
        std::size_t count,
        bool& selection_changed);
    void RemoveDataFromMapRecursively(const std::shared_ptr<Object>& data, bool& selection_changed);

    void CollapseNodeRecursively(TreeNode& node, bool& selection_changed);
    void CollapseNodeWithoutReserveExpandStateRecursively(TreeNode& node, bool& selection_changed);

private:
    std::shared_ptr<TreeNode> root_node_;
    TreeDataMap data_map_;
    TreeDataSet selected_data_set_;
};

}