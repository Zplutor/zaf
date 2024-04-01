#include <zaf/control/internal/tree_control/tree_node_children_adder.h>
#include <zaf/base/error/contract.h>
#include <zaf/control/internal/tree_control/tree_data_manager.h>

namespace zaf::internal {

TreeNodeChildrenAdder::TreeNodeChildrenAdder(
    TreeDataManager& tree_data_manager,
    const std::shared_ptr<TreeNode>& tree_node) 
    :
    tree_data_manager_(tree_data_manager),
    tree_node_(tree_node) {

    ZAF_EXPECT(tree_node_);
}


void TreeNodeChildrenAdder::SetAddRange(std::size_t index, std::size_t count) {

    ZAF_EXPECT(!add_index_ && !add_count_);

    add_index_ = index;
    add_count_ = count;

    tree_data_manager_.AddChildrenToNode(tree_node_, index, count);
}


bool TreeNodeChildrenAdder::MoveToNextChild() {

    ZAF_EXPECT(add_index_ && add_count_);

    if (!current_index_) {
        current_index_ = *add_index_;
    }
    else {
        *current_index_ += 1;
    }

    return *current_index_ < *add_index_ + *add_count_;
}


std::size_t TreeNodeChildrenAdder::GetCurrentChildIndex() const {

    ZAF_EXPECT(current_index_);

    return *current_index_;
}


void TreeNodeChildrenAdder::SetDataToCurrentChild(const std::shared_ptr<Object>& data) {

    ZAF_EXPECT(current_index_);

    tree_data_manager_.SetChildDataToNode(tree_node_, *current_index_, data);
}

}