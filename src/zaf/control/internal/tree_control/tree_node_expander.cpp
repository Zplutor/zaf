#include <zaf/control/internal/tree_control/tree_node_expander.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/control/internal/tree_control/tree_data_manager.h>

namespace zaf::internal {

TreeNodeExpander::TreeNodeExpander(
    TreeDataManager& tree_data_manager, 
    const std::shared_ptr<TreeNode>& tree_node) 
    :
    tree_data_manager_(tree_data_manager),
    tree_node_(tree_node) {

}


void TreeNodeExpander::SetChildCount(std::size_t count) {

    ZAF_EXPECT(!current_child_index_);

    tree_data_manager_.ExpandNodeWithChildCount(*tree_node_, count);
}


bool TreeNodeExpander::MoveToNextChild() {

    if (!current_child_index_) {
        current_child_index_ = 0;
    }
    else {
        *current_child_index_ += 1;
    }

    return *current_child_index_ < tree_node_->children.size();
}


std::size_t TreeNodeExpander::GetCurrentChildIndex() const {

    ZAF_EXPECT(current_child_index_);

    return *current_child_index_;
}


std::shared_ptr<TreeNodeExpander> TreeNodeExpander::GetCurrentChildExpander() {

    ZAF_EXPECT(current_child_index_);

    const auto& child_node = tree_node_->children[*current_child_index_];
    if (!child_node) {
        return nullptr;
    }

    return std::make_shared<TreeNodeExpander>(tree_data_manager_, child_node);
}


void TreeNodeExpander::SetDataToCurrentChild(const std::shared_ptr<Object>& data) {

    ZAF_EXPECT(current_child_index_);

    tree_data_manager_.SetChildDataToNode(tree_node_, *current_child_index_, data);
}

}