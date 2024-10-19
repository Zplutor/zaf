#include <gtest/gtest.h>
#include <zaf/control/list_control.h>
#include <zaf/creation.h>

using namespace zaf;

TEST(ListControlTest, DefaultSelectionMode) {

    auto list = Create<ListControl>();
    ASSERT_EQ(list->SelectionMode(), SelectionMode::Single);
}