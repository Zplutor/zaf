#include <zaf/control/internal/scrollable_control/self_scrolling_layouter.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/scrollable_control.h>
#include <zaf/control/self_scrolling_control.h>

namespace zaf {
namespace internal {

SelfScrollingLayouter::SelfScrollingLayouter(ScrollableControl* scrollable_control) : 
    ScrollableControlLayouter(scrollable_control) {

    auto self_scrolling_control = GetSelfScrollingControl();

    Subscriptions() += self_scrolling_control->ScrollBarChangeEvent().Subscribe(
        std::bind(&SelfScrollingLayouter::SelfScrollingControlScrollBarChange, this));

    Subscriptions() += self_scrolling_control->ScrollValuesChangeEvent().Subscribe(
       std::bind(
           &SelfScrollingLayouter::SelfScrollingControlScrollValuesChange, 
           this, 
           std::placeholders::_1));
}


SelfScrollingLayouter::~SelfScrollingLayouter() {

}


void SelfScrollingLayouter::Layout() {

    auto scrollable_control = GetScrollableControl();
    auto self_scrolling_control = GetSelfScrollingControl();

    bool need_vertical_scroll_bar = 
        scrollable_control->AllowVerticalScroll() && 
        self_scrolling_control->CanShowVerticalScrollBar();

    bool need_horizontal_scroll_bar = 
        scrollable_control->AllowHorizontalScroll() && 
        self_scrolling_control->CanShowHorizontalScrollBar();

    LayoutScrollBars(need_vertical_scroll_bar, need_horizontal_scroll_bar);
    LayoutScrollContainerControl(need_vertical_scroll_bar, need_horizontal_scroll_bar);

    scrollable_control->ScrollContent()->SetRect(
        Rect(Point(),
        scrollable_control->GetScrollContainerControl()->Size()));

    AdjustScrollBarValue(true);
    AdjustScrollBarValue(false);
}


void SelfScrollingLayouter::AdjustScrollBarValue(bool is_horizontal) {

    auto scrollable_control = GetScrollableControl();
    auto self_scrolling_content = GetSelfScrollingControl();

    int current_value{};
    int min_value{};
    int max_value{};
    int page_value{};
    std::shared_ptr<ScrollBar> scroll_bar;

    if (is_horizontal) {

        self_scrolling_content->GetHorizontalScrollValues(
            current_value, 
            min_value, 
            max_value, 
            page_value);

        scroll_bar = GetHorizontalScrollBar();
    }
    else {

        self_scrolling_content->GetVerticalScrollValues(
            current_value, 
            min_value, 
            max_value,
            page_value);

        scroll_bar = GetVerticalScrollBar();
    }

    auto update_guard = scroll_bar->BeginUpdate();
    scroll_bar->SetPageSize(page_value);
    scroll_bar->SetValueRange(min_value, max_value);
    scroll_bar->SetValue(current_value);
}


void SelfScrollingLayouter::ScrollBarScroll(const ScrollBarScrollInfo& event_info) {

    if (is_self_scrolling_) {
        return;
    }

    int value = event_info.ScrollBar()->GetValue();
    if (event_info.ScrollBar()->IsHorizontal()) {
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


void SelfScrollingLayouter::SelfScrollingControlScrollValuesChange(
    const SelfScrollingControlScrollValuesChangeInfo& event_info) {

    is_self_scrolling_ = true;
    AdjustScrollBarValue(event_info.is_horizontal);
    is_self_scrolling_ = false;
}


}
}