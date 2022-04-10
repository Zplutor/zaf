#include <gtest/gtest.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/scrollable_control.h>
#include <zaf/creation.h>

using namespace zaf;

TEST(ScrollableControlTest, AutoScrollContentSize) {

    auto scrollable_control = Create<ScrollableControl>();
    scrollable_control->SetSize(Size{ 100, 100 });
    scrollable_control->SetBorder(Frame{ 0 });
    scrollable_control->SetScrollBarThickness(10);
    scrollable_control->SetAllowHorizontalScroll(true);
    scrollable_control->SetAllowVerticalScroll(true);
    scrollable_control->SetAutoHideScrollBars(false);
    scrollable_control->SetAutoScrollContentSize(true);

    //Initial content size.
    auto scroll_content_control = scrollable_control->ScrollContent();
    ASSERT_EQ(scroll_content_control->Size(), Size(90, 90));

    //Manually set content size would have no effect.
    scroll_content_control->SetSize(Size{ 40, 50 });
    ASSERT_EQ(scroll_content_control->Size(), Size(90, 90));

    //Disable auto size and mannually set size again.
    scrollable_control->SetAutoScrollContentSize(false);
    scroll_content_control->SetSize(Size{ 10, 20 });
    ASSERT_EQ(scroll_content_control->Size(), Size(10, 20));
}


TEST(ScrollableControlTest, EnableScrollBar) {

    auto scrollable_control = Create<ScrollableControl>();
    scrollable_control->SetSize(Size{ 100, 100 });
    scrollable_control->SetAllowHorizontalScroll(true);
    scrollable_control->SetAllowVerticalScroll(true);
    scrollable_control->SetAutoHideScrollBars(false);
    scrollable_control->SetAutoScrollContentSize(false);

    auto scroll_content = scrollable_control->ScrollContent();
    scroll_content->SetSize(Size{ 10, 10 });

    //Scroll bars are disabled if scroll content size is smaller than container.
    ASSERT_FALSE(scrollable_control->VerticalScrollBar()->IsEnabled());
    ASSERT_FALSE(scrollable_control->HorizontalScrollBar()->IsEnabled());

    //Scroll bars are enabled if scroll content size is larger than container.
    scroll_content->SetSize(Size{ 110, 110 });
    ASSERT_TRUE(scrollable_control->VerticalScrollBar()->IsEnabled());
    ASSERT_TRUE(scrollable_control->HorizontalScrollBar()->IsEnabled());
}