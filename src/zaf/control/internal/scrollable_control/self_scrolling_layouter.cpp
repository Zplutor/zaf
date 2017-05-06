#include <zaf/control/internal/scrollable_control/self_scrolling_layouter.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/scrollable_control.h>
#include <zaf/control/self_scrolling_control.h>

namespace zaf {
namespace internal {

SelfScrollingLayouter::SelfScrollingLayouter(ScrollableControl* scrollable_control) : 
    ScrollableControlLayouter(scrollable_control) {

    auto tag = reinterpret_cast<std::uintptr_t>(this);

    GetSelfScrollingControl()->GetScrollBarChangeEvent().AddListenerWithTag(
        tag,
        std::bind(&SelfScrollingLayouter::SelfScrollingControlScrollBarChange, this));

    GetSelfScrollingControl()->GetScrollValuesChangeEvent().AddListenerWithTag(
       tag, 
       std::bind(&SelfScrollingLayouter::SelfScrollingControlScrollValuesChange, this, std::placeholders::_2));
}


SelfScrollingLayouter::~SelfScrollingLayouter() {

    auto tag = reinterpret_cast<std::uintptr_t>(this);
    GetSelfScrollingControl()->GetScrollBarChangeEvent().RemoveListenersWithTag(tag);
    GetSelfScrollingControl()->GetScrollValuesChangeEvent().RemoveListenersWithTag(tag);
}


void SelfScrollingLayouter::Layout() {

    auto scrollable_control = GetScrollableControl();
    auto self_scrolling_control = GetSelfScrollingControl();

    bool can_show_vertical_scroll_bar = 
        scrollable_control->AllowVerticalScroll() && self_scrolling_control->CanShowVerticalScrollBar();

    bool can_show_horizontal_scroll_bar = 
        scrollable_control->AllowHorizontalScroll() && self_scrolling_control->CanShowHorizontalScrollBar();

    LayoutScrollBars(can_show_vertical_scroll_bar, can_show_horizontal_scroll_bar);
    LayoutScrollContainerControl(can_show_vertical_scroll_bar, can_show_horizontal_scroll_bar);

    scrollable_control->GetScrollContentControl()->SetRect(
        Rect(Point(),
        scrollable_control->GetScrollContainerControl()->GetSize()));

    AdjustScrollBarValue(true);
    AdjustScrollBarValue(false);
}


void SelfScrollingLayouter::AdjustScrollBarValue(bool is_horizontal) {

    auto scrollable_control = GetScrollableControl();
    auto self_scrolling_content = GetSelfScrollingControl();

    int current_value = 0;
    int min_value = 0;
    int max_value = 0;
    std::shared_ptr<ScrollBar> scroll_bar;

    if (is_horizontal) {
        self_scrolling_content->GetHorizontalScrollValues(current_value, min_value, max_value);
        scroll_bar = GetHorizontalScrollBar();
    }
    else {
        self_scrolling_content->GetVerticalScrollValues(current_value, min_value, max_value);
        scroll_bar = GetVerticalScrollBar();
    }

    scroll_bar->SetValueRange(min_value, max_value);
    scroll_bar->SetValue(current_value);
}


void SelfScrollingLayouter::ScrollBarScroll(const std::shared_ptr<ScrollBar>& scroll_bar) {

    if (is_self_scrolling_) {
        return;
    }

    int value = scroll_bar->GetValue();
    if (scroll_bar->IsHorizontal()) {
        GetSelfScrollingControl()->HorizontallyScroll(value);
    }
    else {
        GetSelfScrollingControl()->VerticallyScroll(value);
    }
}


void SelfScrollingLayouter::SelfScrollingControlScrollBarChange() {

    Layout();

    GetVerticalScrollBar()->SetIsEnabled(GetSelfScrollingControl()->CanEnableVerticalScrollBar());
    GetHorizontalScrollBar()->SetIsEnabled(GetSelfScrollingControl()->CanEnableHorizontalScrollBar());
}


void SelfScrollingLayouter::SelfScrollingControlScrollValuesChange(bool is_horizontal) {

    is_self_scrolling_ = true;
    AdjustScrollBarValue(is_horizontal);
    is_self_scrolling_ = false;
}


}
}