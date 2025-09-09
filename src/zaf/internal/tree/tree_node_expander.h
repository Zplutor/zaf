#pragma once

#include <optional>
#include <zaf/internal/tree/tree_node.h>

namespace zaf::internal {

class TreeDataManager;

class TreeNodeExpander {
public:
    TreeNodeExpander(
        TreeDataManager& tree_data_manager, 
        const std::shared_ptr<TreeNode>& tree_node);

    TreeNodeExpander(const TreeNodeExpander&) = delete;
    TreeNodeExpander& operator=(const TreeNodeExpander&) = delete;

    const std::shared_ptr<dynamic::Object>& GetNodeData() const {
        return tree_node_->data;
    }

    void SetChildCount(std::size_t count);

    bool MoveToNextChild();
    std::size_t GetCurrentChildIndex() const;

    std::shared_ptr<TreeNodeExpander> GetCurrentChildExpander();

    void SetDataToCurrentChild(const std::shared_ptr<dynamic::Object>& data);

private:
    TreeDataManager& tree_data_manager_;
    std::shared_ptr<TreeNode> tree_node_;

    std::optional<std::size_t> current_child_index_;
};

}