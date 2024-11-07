#include <gtest/gtest.h>
#include <zaf/control/tree_control.h>

using namespace zaf;

/**
Make sure there is no circular reference inside the tree control.
*/
TEST(TreeControlTest, CircularReference) {

    auto tree = Create<TreeControl>();
    std::weak_ptr<TreeControl> weak_ptr = tree;

    tree.reset();
    ASSERT_TRUE(weak_ptr.expired());
}