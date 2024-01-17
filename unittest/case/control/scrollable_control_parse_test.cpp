#include <gtest/gtest.h>
#include <zaf/base/as.h>
#include <zaf/base/error/basic_error.h>
#include <zaf/control/label.h>
#include <zaf/control/scrollable_control.h>
#include "case/parsing/parsers/utility.h"
#include "utility/assert.h"

using namespace zaf;

TEST(ScrollableControlParseTest, AllowVerticalScroll) {

    ASSERT_TRUE(TestBooleanProperty<ScrollBox>(
        "AllowVerticalScroll", 
        [](ScrollBox& scrollable_control) {
            return scrollable_control.AllowVerticalScroll();
        }
    ));
}


TEST(ScrollableControlParseTest, AllowHorizontalScroll) {

    ASSERT_TRUE(TestBooleanProperty<ScrollBox>(
        "AllowHorizontalScroll",
        [](ScrollBox& scrollable_control) {
            return scrollable_control.AllowHorizontalScroll();
        }
    ));
}


TEST(ScrollableControlParseTest, AutoHideScrollBars) {

    ASSERT_TRUE(TestBooleanProperty<ScrollBox>(
        "AutoHideScrollBars",
        [](ScrollBox& scrollable_control) {
            return scrollable_control.AutoHideScrollBars();
        }
    ));
}


TEST(ScrollableControlParseTest, AutoScrollBarLargeChange) {

    ASSERT_TRUE(TestBooleanProperty<ScrollBox>(
        "AutoScrollBarLargeChange",
        [](ScrollBox& scrollable_control) {
            return scrollable_control.AutoScrollBarLargeChange();
        }
    ));
}


TEST(ScrollableControlParseTest, AutoScrollContentHeight) {

    ASSERT_TRUE(TestBooleanProperty<ScrollBox>(
        "AutoScrollContentHeight",
        [](ScrollBox& scrollable_control) {
            return scrollable_control.AutoScrollContentHeight();
        }
    ));
}


TEST(ScrollableControlParseTest, AutoScrollContentWidth) {

    ASSERT_TRUE(TestBooleanProperty<ScrollBox>(
        "AutoScrollContentWidth",
        [](ScrollBox& scrollable_control) {
            return scrollable_control.AutoScrollContentWidth();
        }
    ));
}


TEST(ScrollableControlParseTest, ScrollContent) {

    auto xaml = LR"(
        <ScrollBox>
            <ScrollBox.ScrollContent>
                <Label Text="ScrollContent" />
            </ScrollBox.ScrollContent>
        </ScrollBox>
    )";

    auto scrollable_control = CreateObjectFromXaml<ScrollBox>(xaml);
    ASSERT_NE(scrollable_control, nullptr);

    auto label = As<Label>(scrollable_control->ScrollContent());
    ASSERT_NE(label, nullptr);
    ASSERT_EQ(label->Text(), L"ScrollContent");
}


TEST(ScrollableControlParseTest, ContentNode) {

    {
        auto xaml = LR"(
            <ScrollBox>
                <Label Text="ContentNode" />
            </ScrollBox>
        )";
        auto scrollable_control = CreateObjectFromXaml<ScrollBox>(xaml);
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
            <ScrollBox>
            </ScrollBox>
        )";
        auto scrollable_control = CreateObjectFromXaml<ScrollBox>(xaml);
        ASSERT_NE(scrollable_control, nullptr);
    }

    //Multiple content nodes are not allowed.
    {
        auto xaml = LR"(
            <ScrollBox>
                <Label />
                <Label />
            </ScrollBox>
        )";
        ASSERT_THROW_ERRC(CreateObjectFromXaml<ScrollBox>(xaml), BasicErrc::InvalidValue);
    }

    //Invalid object.
    {
        auto xaml = LR"(
            <ScrollBox>
                <NoThisObject />
            </ScrollBox>
        )";
        ASSERT_THROW_ERRC(CreateObjectFromXaml<ScrollBox>(xaml), BasicErrc::InvalidValue);
    }
}