#include <zaf/control/internal/scrollable_control/scrollable_control_layouter.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/scrollable_control.h>

namespace zaf {
namespace internal {

ScrollableControlLayouter::ScrollableControlLayouter(ScrollableControl* scrollable_control) : 
    scrollable_control_(scrollable_control) {

    RegisterScrollBarEvent(GetVerticalScrollBar(), false);
    RegisterScrollBarEvent(GetHorizontalScrollBar(), true);
}


ScrollableControlLayouter::~ScrollableControlLayouter() {

    UnregisterScrollBarEvent(true);
    UnregisterScrollBarEvent(false);
}


void ScrollableControlLayouter::ScrollBarChange(
    bool is_horizontal, 
    const std::shared_ptr<ScrollBar>& previous_scroll_bar) {

    UnregisterScrollBarEvent(is_horizontal);

    auto new_scroll_bar = is_horizontal ? GetHorizontalScrollBar() : GetVerticalScrollBar();
    RegisterScrollBarEvent(new_scroll_bar, is_horizontal);
}


void ScrollableControlLayouter::RegisterScrollBarEvent(
    const std::shared_ptr<ScrollBar>& scroll_bar, 
    bool is_horizontal) {

    auto& subscription =
        is_horizontal ?
        horizontal_scroll_bar_subscription_ :
        vertical_scroll_bar_subscription_;

    subscription = scroll_bar->ScrollEvent().Subscribe(
        std::bind(&ScrollableControlLayouter::ScrollBarScroll, this, std::placeholders::_1));
}


void ScrollableControlLayouter::UnregisterScrollBarEvent(bool is_horizontal) {

    auto& subscription = 
        is_horizontal ? 
        horizontal_scroll_bar_subscription_ : 
        vertical_scroll_bar_subscription_;

    subscription.Unsubscribe();
}


void ScrollableControlLayouter::LayoutScrollBars(bool can_show_vertical_scroll_bar, bool can_show_horizontal_scroll_bar) {

    auto content_size = GetScrollableControl()->GetContentSize();
    float scroll_bar_thickness = GetScrollableControl()->GetScrollBarThickness();

    //Layout vertical scroll bar
    GetVerticalScrollBar()->SetIsVisible(can_show_vertical_scroll_bar);
    if (can_show_vertical_scroll_bar) {

        Rect vertical_scroll_bar_rect(
            content_size.width - scroll_bar_thickness,
            0,
            scroll_bar_thickness,
            content_size.height - (can_show_horizontal_scroll_bar ? scroll_bar_thickness : 0)
        );
        GetVerticalScrollBar()->SetRect(vertical_scroll_bar_rect);
    }

    //Layout horizontal scroll bar
    GetHorizontalScrollBar()->SetIsVisible(can_show_horizontal_scroll_bar);
    if (can_show_horizontal_scroll_bar) {

        Rect horizontal_scroll_bar_rect(
            0,
            content_size.height - scroll_bar_thickness,
            content_size.width - (can_show_vertical_scroll_bar ? scroll_bar_thickness : 0),
            scroll_bar_thickness
        );
        GetHorizontalScrollBar()->SetRect(horizontal_scroll_bar_rect);
    }

    //Layout scroll bar corner
    const auto& corner = GetScrollableControl()->GetScrollBarCorner();
    if (can_show_vertical_scroll_bar && can_show_horizontal_scroll_bar) {

        corner->SetIsVisible(true);

        Rect corner_rect(
            content_size.width - scroll_bar_thickness,
            content_size.height - scroll_bar_thickness,
            scroll_bar_thickness,
            scroll_bar_thickness);

        corner->SetRect(corner_rect);
    }
    else {
        corner->SetIsVisible(false);
    }
}


void ScrollableControlLayouter::LayoutScrollContainerControl(bool can_show_vertical_scroll_bar, bool can_show_horizontal_scroll_bar) {

    Rect rect(Point(), GetScrollableControl()->GetContentSize());

    if (can_show_vertical_scroll_bar) {
        rect.size.width -= GetVerticalScrollBar()->Width();
    }

    if (can_show_horizontal_scroll_bar) {
        rect.size.height -= GetHorizontalScrollBar()->Height();
    }

    GetScrollableControl()->GetScrollContainerControl()->SetRect(rect);
}

}
}