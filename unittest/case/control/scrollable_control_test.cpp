#include <gtest/gtest.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/scrollable_control.h>
#include <zaf/creation.h>
#include <zaf/graphic/dpi.h>
#include <zaf/window/window.h>

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


TEST(ScrollableControlTest, ReserveScrollContentPositionAfterLayout) {

    auto scrollable_control = Create<ScrollableControl>();
    scrollable_control->SetSize(Size{ 110, 110 });
    scrollable_control->SetAllowHorizontalScroll(true);
    scrollable_control->SetAllowVerticalScroll(true);
    scrollable_control->SetAutoScrollContentSize(true);
    scrollable_control->SetScrollBarThickness(10);

    auto scroll_content = scrollable_control->ScrollContent();
    scroll_content->SetFixedSize(Size{ 200, 200 });

    scrollable_control->HorizontalScrollBar()->SetValue(50);
    scrollable_control->VerticalScrollBar()->SetValue(50);

    ASSERT_EQ(scroll_content->X(), -50);
    ASSERT_EQ(scroll_content->Y(), -50);

    scrollable_control->SetSize(Size{ 150, 150 });
    ASSERT_EQ(scroll_content->X(), -50);
    ASSERT_EQ(scroll_content->Y(), -50);
}


TEST(ScrollableControlTest, MouseWheelEvent) {

    auto window = Create<Window>();
    window->SetInitialRectStyle(InitialRectStyle::Custom);
    window->SetPosition({});
    window->SetContentSize(Size{ 200, 200 });

    auto scrollable_control = Create<ScrollableControl>();
    scrollable_control->SetSize(Size{ 110, 110 });
    scrollable_control->SetBorder({});
    scrollable_control->SetAllowHorizontalScroll(true);
    scrollable_control->SetAllowVerticalScroll(true);
    scrollable_control->SetScrollBarThickness(10);
    scrollable_control->SetAutoScrollContentSize(false);

    window->RootControl()->AddChild(scrollable_control);

    auto scroll_content = Create<Control>();
    scrollable_control->SetScrollContent(scroll_content);

    bool is_handled{};
    auto subscription = window->RootControl()->MouseWheelEvent().Subscribe(
        [&](const zaf::MouseWheelInfo& event_info) {

        is_handled = event_info.IsHandled();
    });

    window->Show();

    auto position_in_screen = FromDIPs(
        window->ToScreenPosition(Point{ 50, 50 }),
        window->GetDPI());

    auto lparam = MAKELPARAM(
        static_cast<WORD>(position_in_screen.x),
        static_cast<WORD>(position_in_screen.y));

    //MouseWheelEvent should be handled if ScrollableControl handles it.
    {
        scroll_content->SetFixedSize({ 200, 200 });

        is_handled = false;
        ::SendMessage(window->Handle(), WM_MOUSEWHEEL, MAKEWPARAM(0, 120), lparam);
        ASSERT_TRUE(is_handled);

        is_handled = false;
        ::SendMessage(window->Handle(), WM_MOUSEHWHEEL, MAKEWPARAM(0, 120), lparam);
        ASSERT_TRUE(is_handled);
    }

    //MouseWheelEvent shouldn't be handled if ScrollableControl doesn't handle it.
    {
        scroll_content->SetFixedSize({ 100, 100 });

        is_handled = false;
        ::SendMessage(window->Handle(), WM_MOUSEWHEEL, MAKEWPARAM(0, 120), lparam);
        ASSERT_FALSE(is_handled);

        is_handled = false;
        ::SendMessage(window->Handle(), WM_MOUSEHWHEEL, MAKEWPARAM(0, 120), lparam);
        ASSERT_FALSE(is_handled);
    }

    subscription.Unsubscribe();
    window->Destroy();
}