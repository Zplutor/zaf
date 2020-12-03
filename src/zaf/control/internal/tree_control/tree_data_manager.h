#pragma once

#include <optional>
#include <unordered_map>
#include <zaf/base/define.h>
#include <zaf/control/internal/tree_control/tree_data_helpers.h>

namespace zaf::internal {

class TreeNode {
public:
    std::shared_ptr<Object> data;

    std::weak_ptr<TreeNode> parent;
    std::size_t index_in_parent{ InvalidIndex };

    std::optional<std::vector<std::shared_ptr<TreeNode>>> children;
};

class TreeDataManager {
public:
    TreeDataManager();

    TreeDataManager(const TreeDataManager&) = delete;
    TreeDataManager& operator=(const TreeDataManager&) = delete;

    std::shared_ptr<const TreeNode> GetNodeAtIndexPath(const IndexPath& index_path) const;
    std::optional<IndexPath> GetIndexPathOfData(const std::shared_ptr<Object>& data) const;

    void AddNode(
        const std::shared_ptr<Object>& parent_data,
        std::size_t index_in_parent, 
        const std::shared_ptr<Object>& data);

    void SetChildCount(const std::shared_ptr<Object>& data, std::size_t child_count);

    void AddChildren(
        const std::shared_ptr<Object>& parent_data, 
        std::size_t child_index,
        std::size_t count);

    std::vector<std::shared_ptr<Object>> RemoveChildren(
        const std::shared_ptr<Object>& parent_data,
        std::size_t child_index, 
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
    void Initialize();
    void RemoveDataFromMapRecursively(
        const std::shared_ptr<Object>& data,
        std::vector<std::shared_ptr<Object>>& removed_data_list);

private:
    std::shared_ptr<TreeNode> root_node_;
    TreeDataMap data_map_;
};

}