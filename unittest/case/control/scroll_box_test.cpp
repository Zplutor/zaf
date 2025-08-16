#include <gtest/gtest.h>
#include <zaf/base/as.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/scroll_box.h>
#include <zaf/creation.h>
#include <zaf/graphic/dpi.h>
#include <zaf/rx/disposable.h>
#include <zaf/window/window.h>

using namespace zaf;

TEST(ScrollBoxTest, AutoScrollContentSize) {

    auto scrollable_control = Create<ScrollBox>();
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


//Simulates that the height changs when the width changes.
class AutoSizeControl : public Control {
protected:
    void OnRectChanged(const RectChangedInfo& event_info) override {

        __super::OnRectChanged(event_info);

        if (this->Width() != event_info.PreviousRect().size.width) {
            this->SetFixedHeight(this->Width() * 2);
        }
    }
};

//Verifies that the scroll value remains correct when the size of the scroll content changes during
//ScrollBox layout.
TEST(ScrollBoxTest, ChangeScrollContentSizeDuringLayout) {

    auto scroll_content = Create<AutoSizeControl>();
    auto scrollable_control = Create<ScrollBox>();
    scrollable_control->SetBorder({});
    scrollable_control->SetScrollBarThickness(10);
    scrollable_control->SetScrollContent(scroll_content);
    scrollable_control->SetSize(Size{ 100, 100 });

    ASSERT_EQ(scrollable_control->VerticalScrollBar()->MaxValue(), 90);
}


TEST(ScrollBoxTest, EnableScrollBar) {

    auto scrollable_control = Create<ScrollBox>();
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


TEST(ScrollBoxTest, ReserveScrollContentPositionAfterLayout) {

    auto scrollable_control = Create<ScrollBox>();
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


TEST(ScrollBoxTest, MouseWheelEvent) {

    auto window = Create<Window>();
    window->SetInitialRectStyle(InitialRectStyle::Custom);
    window->SetPosition({});
    window->SetContentSize(Size{ 200, 200 });

    auto scrollable_control = Create<ScrollBox>();
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
        window->TranslateToScreen(Point{ 50, 50 }),
        window->GetDPI());

    auto lparam = MAKELPARAM(
        static_cast<WORD>(position_in_screen.x),
        static_cast<WORD>(position_in_screen.y));

    //MouseWheelEvent should be handled if ScrollBox handles it.
    {
        scroll_content->SetFixedSize({ 200, 200 });

        is_handled = false;
        ::SendMessage(window->Handle(), WM_MOUSEWHEEL, MAKEWPARAM(0, 120), lparam);
        ASSERT_TRUE(is_handled);

        is_handled = false;
        ::SendMessage(window->Handle(), WM_MOUSEHWHEEL, MAKEWPARAM(0, 120), lparam);
        ASSERT_TRUE(is_handled);
    }

    //MouseWheelEvent shouldn't be handled if ScrollBox doesn't handle it.
    {
        scroll_content->SetFixedSize({ 100, 100 });

        is_handled = false;
        ::SendMessage(window->Handle(), WM_MOUSEWHEEL, MAKEWPARAM(0, 120), lparam);
        ASSERT_FALSE(is_handled);

        is_handled = false;
        ::SendMessage(window->Handle(), WM_MOUSEHWHEEL, MAKEWPARAM(0, 120), lparam);
        ASSERT_FALSE(is_handled);
    }

    subscription->Dispose();
    window->Destroy();
}


TEST(ScrollBoxTest, UseOverlayScrollBars) {

    auto scroll_control = zaf::Create<zaf::ScrollBox>();
    scroll_control->SetAutoHideScrollBars(false);
    scroll_control->SetBorder({});
    scroll_control->SetPadding({});
    scroll_control->SetSize(zaf::Size{ 100, 100 });
    scroll_control->SetScrollBarThickness(10);

    //No overlay scroll bars
    scroll_control->SetUseOverlayScrollBars(false);
    ASSERT_EQ(scroll_control->ScrollContent()->Rect(), zaf::Rect(0, 0, 90, 90));
    ASSERT_EQ(scroll_control->HorizontalScrollBar()->Rect(), zaf::Rect(0, 90, 90, 10));
    ASSERT_EQ(scroll_control->VerticalScrollBar()->Rect(), zaf::Rect(90, 0, 10, 90));
    ASSERT_EQ(scroll_control->ScrollBarCorner()->Rect(), zaf::Rect(90, 90, 10, 10));

    //Use overlay scroll bars
    scroll_control->SetUseOverlayScrollBars(true);
    ASSERT_EQ(scroll_control->ScrollContent()->Rect(), zaf::Rect(0, 0, 100, 100));
    ASSERT_EQ(scroll_control->HorizontalScrollBar()->Rect(), zaf::Rect(0, 90, 90, 10));
    ASSERT_EQ(scroll_control->VerticalScrollBar()->Rect(), zaf::Rect(90, 0, 10, 90));
    ASSERT_EQ(scroll_control->ScrollBarCorner()->Rect(), zaf::Rect(90, 90, 10, 10));
}


TEST(ScrollBoxTest, TranslateToScrollContent) {

    auto scroll_box = zaf::Create<zaf::ScrollBox>();
    scroll_box->SetSize(zaf::Size{ 100, 100 });
    scroll_box->SetBorder(zaf::Frame{ 2, 4, 2, 4 });
    scroll_box->SetPadding(zaf::Frame{ 1, 3, 1, 3 });
    scroll_box->SetScrollBarThickness(10);
    scroll_box->ScrollContent()->SetFixedSize(zaf::Size{ 200, 200 });

    zaf::Point position_in_box{ 20, 20 };

    auto translated_position = scroll_box->TranslateToScrollContent(position_in_box);
    ASSERT_EQ(translated_position, zaf::Point(17, 13));

    scroll_box->ScrollToRight();
    translated_position = scroll_box->TranslateToScrollContent(position_in_box);
    ASSERT_EQ(translated_position, zaf::Point(133, 13));

    scroll_box->ScrollToBottom();
    translated_position = scroll_box->TranslateToScrollContent(position_in_box);
    ASSERT_EQ(translated_position, zaf::Point(133, 137));
}


TEST(ScrollBoxTest, ScrollTo) {

    auto scroll_box = zaf::Create<zaf::ScrollBox>();
    scroll_box->SetSize(zaf::Size{ 100, 100 });
    scroll_box->SetBorder({});
    scroll_box->SetScrollBarThickness(10);
    
    auto content = zaf::Create<zaf::Control>();
    content->SetFixedSize(zaf::Size{ 200, 200 });
    scroll_box->SetScrollContent(content);

    const auto& vertical_scroll_bar = scroll_box->VerticalScrollBar();
    const auto& horizontal_scroll_bar = scroll_box->HorizontalScrollBar();

    scroll_box->ScrollToBottom();
    ASSERT_EQ(vertical_scroll_bar->Value(), 110);
    ASSERT_EQ(horizontal_scroll_bar->Value(), 0);

    scroll_box->ScrollToRight();
    ASSERT_EQ(vertical_scroll_bar->Value(), 110);
    ASSERT_EQ(horizontal_scroll_bar->Value(), 110);

    scroll_box->ScrollToTop();
    ASSERT_EQ(vertical_scroll_bar->Value(), 0);
    ASSERT_EQ(horizontal_scroll_bar->Value(), 110);

    scroll_box->ScrollToLeft();
    ASSERT_EQ(vertical_scroll_bar->Value(), 0);
    ASSERT_EQ(horizontal_scroll_bar->Value(), 0);
}


TEST(ScrollBoxTest, ViewportRect) {

    auto scroll_box = Create<ScrollBox>();
    scroll_box->SetSize(Size{ 100, 100 });
    scroll_box->SetBorder(Frame{ 1 });
    scroll_box->SetPadding(Frame{ 2 });
    scroll_box->SetVerticalScrollBarThickness(20);
    scroll_box->SetHorizontalScrollBarThickness(10);

    ASSERT_EQ(scroll_box->ViewportRect(), Rect(3, 3, 74, 84 ));
}