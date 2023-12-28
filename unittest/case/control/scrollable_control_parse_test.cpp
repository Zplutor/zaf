#include <gtest/gtest.h>
#include <zaf/base/as.h>
#include <zaf/base/error/basic_error.h>
#include <zaf/control/label.h>
#include <zaf/control/scrollable_control.h>
#include "case/parsing/parsers/utility.h"
#include "utility/assert.h"

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


TEST(ScrollableControlParseTest, AutoScrollBarLargeChange) {

    ASSERT_TRUE(TestBooleanProperty<ScrollableControl>(
        "AutoScrollBarLargeChange",
        [](ScrollableControl& scrollable_control) {
            return scrollable_control.AutoScrollBarLargeChange();
        }
    ));
}


TEST(ScrollableControlParseTest, AutoScrollContentHeight) {

    ASSERT_TRUE(TestBooleanProperty<ScrollableControl>(
        "AutoScrollContentHeight",
        [](ScrollableControl& scrollable_control) {
            return scrollable_control.AutoScrollContentHeight();
        }
    ));
}


TEST(ScrollableControlParseTest, AutoScrollContentWidth) {

    ASSERT_TRUE(TestBooleanProperty<ScrollableControl>(
        "AutoScrollContentWidth",
        [](ScrollableControl& scrollable_control) {
            return scrollable_control.AutoScrollContentWidth();
        }
    ));
}


TEST(ScrollableControlParseTest, ScrollContent) {

    auto xaml = LR"(
        <ScrollableControl>
            <ScrollableControl.ScrollContent>
                <Label Text="ScrollContent" />
            </ScrollableControl.ScrollContent>
        </ScrollableControl>
    )";

    auto scrollable_control = CreateObjectFromXaml<ScrollableControl>(xaml);
    ASSERT_NE(scrollable_control, nullptr);

    auto label = As<Label>(scrollable_control->ScrollContent());
    ASSERT_NE(label, nullptr);
    ASSERT_EQ(label->Text(), L"ScrollContent");
}


TEST(ScrollableControlParseTest, ContentNode) {

    {
        auto xaml = LR"(
            <ScrollableControl>
                <Label Text="ContentNode" />
            </ScrollableControl>
        )";
        auto scrollable_control = CreateObjectFromXaml<ScrollableControl>(xaml);
        ASSERT_NE(scrollable_control, nullptr);
        auto label = As<Label>(scrollable_control->ScrollContent());
        ASSERT_NE(label, nullptr);
        ASSERT_EQ(label->Text(), L"ContentNode");
        //Make sure the content node will not be added to children.
        ASSERT_EQ(scrollable_control->ChildCount(), 4);
    }

    //Empty content nodes.
    {
        auto xaml = LR"(
            <ScrollableControl>
            </ScrollableControl>
        )";
        auto scrollable_control = CreateObjectFromXaml<ScrollableControl>(xaml);
        ASSERT_NE(scrollable_control, nullptr);
    }

    //Multiple content nodes are not allowed.
    {
        auto xaml = LR"(
            <ScrollableControl>
                <Label />
                <Label />
            </ScrollableControl>
        )";
        ASSERT_THROW_ERRC(CreateObjectFromXaml<ScrollableControl>(xaml), BasicErrc::InvalidValue);
    }

    //Invalid object.
    {
        auto xaml = LR"(
            <ScrollableControl>
                <NoThisObject />
            </ScrollableControl>
        )";
        ASSERT_THROW_ERRC(CreateObjectFromXaml<ScrollableControl>(xaml), BasicErrc::InvalidValue);
    }
}