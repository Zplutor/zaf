#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/dynamic/object.h>

namespace zaf::internal {

class TreeVisitor : NonCopyableNonMovable {
public:
    TreeVisitor() = default;
    virtual ~TreeVisitor() = default;

    virtual void VisitNode(const std::shared_ptr<dynamic::Object>& node_data) = 0;
    virtual void EnterNode() = 0;
    virtual void ExitNode() = 0;
};

}