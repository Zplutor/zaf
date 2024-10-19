#include <gtest/gtest.h>
#include <zaf/control/list_control.h>
#include <zaf/control/list_item_container.h>
#include <zaf/creation.h>

using namespace zaf;

TEST(ListControlTest, DefaultFocus) {

    auto list = Create<ListControl>();
    ASSERT_TRUE(list->CanFocused());
    ASSERT_FALSE(list->ItemContainer()->CanFocused());
}