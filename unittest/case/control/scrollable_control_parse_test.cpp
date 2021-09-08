#include <gtest/gtest.h>
#include <zaf/control/scrollable_control.h>
#include "case/parsing/parsers/utility.h"

using namespace zaf;

TEST(ScrollableControlParseTest, AllowVerticalScroll) {

    ASSERT_TRUE(TestBooleanProperty<ScrollableControl>(
        "AllowVerticalScroll", 
        [](ScrollableControl& scrollable_control) {
            return scrollable_control.AllowVerticalScroll();
        }
    ));
}


TEST(ScrollableControlParseTest, AllowHorizontalScroll) {

    ASSERT_TRUE(TestBooleanProperty<ScrollableControl>(
        "AllowHorizontalScroll",
        [](ScrollableControl& scrollable_control) {
            return scrollable_control.AllowHorizontalScroll();
        }
    ));
}


TEST(ScrollableControlParseTest, AutoHideScrollBars) {

    ASSERT_TRUE(TestBooleanProperty<ScrollableControl>(
        "AutoHideScrollBars",
        [](ScrollableControl& scrollable_control) {
        return scrollable_control.AutoHideScrollBars();
    }
    ));
}