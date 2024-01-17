#include <gtest/gtest.h>
#include <zaf/base/as.h>
#include <zaf/base/error/basic_error.h>
#include <zaf/control/label.h>
#include <zaf/control/scroll_box.h>
#include "case/parsing/parsers/utility.h"
#include "utility/assert.h"

using namespace zaf;

TEST(ScrollBoxParseTest, AllowVerticalScroll) {

    ASSERT_TRUE(TestBooleanProperty<ScrollBox>(
        "AllowVerticalScroll", 
        [](ScrollBox& scrollable_control) {
            return scrollable_control.AllowVerticalScroll();
        }
    ));
}


TEST(ScrollBoxParseTest, AllowHorizontalScroll) {

    ASSERT_TRUE(TestBooleanProperty<ScrollBox>(
        "AllowHorizontalScroll",
        [](ScrollBox& scrollable_control) {
            return scrollable_control.AllowHorizontalScroll();
        }
    ));
}


TEST(ScrollBoxParseTest, AutoHideScrollBars) {

    ASSERT_TRUE(TestBooleanProperty<ScrollBox>(
        "AutoHideScrollBars",
        [](ScrollBox& scrollable_control) {
            return scrollable_control.AutoHideScrollBars();
        }
    ));
}


TEST(ScrollBoxParseTest, AutoScrollBarLargeChange) {

    ASSERT_TRUE(TestBooleanProperty<ScrollBox>(
        "AutoScrollBarLargeChange",
        [](ScrollBox& scrollable_control) {
            return scrollable_control.AutoScrollBarLargeChange();
        }
    ));
}


TEST(ScrollBoxParseTest, AutoScrollContentHeight) {

    ASSERT_TRUE(TestBooleanProperty<ScrollBox>(
        "AutoScrollContentHeight",
        [](ScrollBox& scrollable_control) {
            return scrollable_control.AutoScrollContentHeight();
        }
    ));
}


TEST(ScrollBoxParseTest, AutoScrollContentWidth) {

    ASSERT_TRUE(TestBooleanProperty<ScrollBox>(
        "AutoScrollContentWidth",
        [](ScrollBox& scrollable_control) {
            return scrollable_control.AutoScrollContentWidth();
        }
    ));
}


TEST(ScrollBoxParseTest, ScrollContent) {

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


TEST(ScrollBoxParseTest, ContentNode) {

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