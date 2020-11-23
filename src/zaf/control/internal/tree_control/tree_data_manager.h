#pragma once

#include <optional>
#include <unordered_map>
#include <zaf/base/define.h>
#include <zaf/control/internal/tree_control/tree_data_helpers.h>

namespace zaf::internal {

class TreeNode {
public:
    std::shared_ptr<Object> data;
    std::size_t child_count{};

    std::weak_ptr<TreeNode> parent;
    std::size_t index_in_parent{ InvalidIndex };

    std::vector<std::shared_ptr<TreeNode>> children;
};

class TreeDataManager {
public:
    TreeDataManager();

    TreeDataManager(const TreeDataManager&) = delete;
    TreeDataManager& operator=(const TreeDataManager&) = delete;

    std::shared_ptr<const TreeNode> GetNodeAtIndexPath(const IndexPath& index_path) const;
    std::optional<IndexPath> GetIndexPathOfData(const std::shared_ptr<Object>& data);

    void AddChildren(
        const std::shared_ptr<Object>& parent_data, 
        std::size_t parent_index,
        std::size_t count);

    void AddNewNode(
        const std::shared_ptr<Object>& parent_data,
        std::size_t parent_index, 
        const std::shared_ptr<Object>& data,
        std::size_t child_count);

    void RemoveChildren(
        const std::shared_ptr<Object>& parent_data,
        std::size_t parent_index, 
        std::size_t count);

    void Clear();

private:
    using TreeDataMap = std::unordered_map<
        std::shared_ptr<Object>,
        std::shared_ptr<TreeNode>,
        TreeDataHash,
        TreeDataEqual
    >;

private:
    void AddRootNodeToMap();
    void RemoveDataFromMapRecursively(const std::shared_ptr<Object>& data);

private:
    std::shared_ptr<TreeNode> root_node_;
    TreeDataMap data_map_;
};

}