#pragma once

#include <zaf/base/define.h>
#include <zaf/object/object.h>

namespace zaf::internal {

class TreeNode {
public:
    TreeNode() = default;

    TreeNode(const TreeNode&) = delete;
    TreeNode& operator=(const TreeNode&) = delete;

    std::shared_ptr<Object> data;

    std::weak_ptr<TreeNode> parent;
    std::size_t index_in_parent{ InvalidIndex };

    std::vector<std::shared_ptr<TreeNode>> children;
    bool is_expanded{};
};


}