#pragma once

#include <optional>
#include <zaf/internal/tree/tree_node.h>

namespace zaf::internal {

class TreeDataManager;

class TreeNodeChildrenAdder {
public:
    TreeNodeChildrenAdder(
        TreeDataManager& tree_data_manager,
        const std::shared_ptr<TreeNode>& tree_node);

    TreeNodeChildrenAdder(const TreeNodeChildrenAdder&) = delete;
    TreeNodeChildrenAdder& operator=(const TreeNodeChildrenAdder&) = delete;

    void SetAddRange(std::size_t index, std::size_t count);

    bool MoveToNextChild();
    std::size_t GetCurrentChildIndex() const;
    void SetDataToCurrentChild(const std::shared_ptr<dynamic::Object>& data);

private:
    TreeDataManager& tree_data_manager_;
    std::shared_ptr<TreeNode> tree_node_;

    std::optional<std::size_t> add_index_;
    std::optional<std::size_t> add_count_;

    std::optional<std::size_t> current_index_;
};

}