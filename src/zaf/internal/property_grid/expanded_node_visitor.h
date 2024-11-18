#pragma once

#include <stack>
#include <zaf/internal/tree/tree_visitor.h>
#include <zaf/control/property_grid_node.h>

namespace zaf::internal {

class ExpandedNodeVisitor : public zaf::internal::TreeVisitor {
public:
    ExpandedNodeVisitor();

    PropertyGridNode TakeResult() {
        return std::move(root_node_);
    }

    void VisitNode(const std::shared_ptr<Object>& node_data) override;
    void EnterNode() override;
    void ExitNode() override;

private:
    PropertyGridNode root_node_;
    PropertyGridNode* last_visited_node_{};
    std::stack<PropertyGridNode*> visiting_node_stack_;
};

}