#include <zaf/internal/property_grid/expanded_node_visitor.h>
#include <zaf/base/as.h>
#include <zaf/internal/property_grid/property_data.h>

namespace zaf::internal {

ExpandedNodeVisitor::ExpandedNodeVisitor() : root_node_(nullptr) {

}


void ExpandedNodeVisitor::VisitNode(const std::shared_ptr<Object>& node_data) {

    auto data = As<PropertyData>(node_data);
    if (data) {

        auto visiting_node = visiting_node_stack_.top();
        last_visited_node_ = visiting_node->AddChild(data->Property());
    }
    else {

        last_visited_node_ = &root_node_;
    }
}


void ExpandedNodeVisitor::EnterNode() {

    ZAF_EXPECT(last_visited_node_);

    visiting_node_stack_.push(last_visited_node_);
}


void ExpandedNodeVisitor::ExitNode() {

    visiting_node_stack_.pop();
}

}