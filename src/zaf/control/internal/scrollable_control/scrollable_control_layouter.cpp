#include <zaf/control/internal/scrollable_control/scrollable_control_layouter.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/scrollable_control.h>

namespace zaf::internal {

ScrollableControlLayouter::ScrollableControlLayouter(ScrollBox* scrollable_control) : 
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


void ScrollableControlLayouter::LayoutScrollBars(
    bool need_vertical_scroll_bar, 
    bool need_horizontal_scroll_bar) {

    auto content_size = GetScrollableControl()->ContentSize();
    float vertical_scroll_bar_thickness = GetScrollableControl()->VerticalScrollBarThickness();
    float horizontal_scroll_bar_thickness = GetScrollableControl()->HorizontalScrollBarThickness();

    //Layout vertical scroll bar
    GetVerticalScrollBar()->SetIsVisible(need_vertical_scroll_bar);
    if (need_vertical_scroll_bar) {

        auto vertical_scroll_bar_height =
            content_size.height - 
            (need_horizontal_scroll_bar ? horizontal_scroll_bar_thickness : 0);

        Rect vertical_scroll_bar_rect(
            content_size.width - vertical_scroll_bar_thickness,
            0,
            vertical_scroll_bar_thickness,
            vertical_scroll_bar_height
        );
        GetVerticalScrollBar()->SetRect(vertical_scroll_bar_rect);
    }

    //Layout horizontal scroll bar
    GetHorizontalScrollBar()->SetIsVisible(need_horizontal_scroll_bar);
    if (need_horizontal_scroll_bar) {

        Rect horizontal_scroll_bar_rect(
            0,
            content_size.height - horizontal_scroll_bar_thickness,
            content_size.width - (need_vertical_scroll_bar ? vertical_scroll_bar_thickness : 0),
            horizontal_scroll_bar_thickness
        );
        GetHorizontalScrollBar()->SetRect(horizontal_scroll_bar_rect);
    }

    //Layout scroll bar corner
    const auto& corner = GetScrollableControl()->ScrollBarCorner();
    if (need_vertical_scroll_bar && need_horizontal_scroll_bar) {

        corner->SetIsVisible(true);

        Rect corner_rect(
            content_size.width - vertical_scroll_bar_thickness,
            content_size.height - horizontal_scroll_bar_thickness,
            vertical_scroll_bar_thickness,
            horizontal_scroll_bar_thickness);

        corner->SetRect(corner_rect);
    }
    else {
        corner->SetIsVisible(false);
    }
}


void ScrollableControlLayouter::LayoutScrollContainerControl(
    bool need_vertical_scroll_bar, 
    bool need_horizontal_scroll_bar) {

    Rect rect(Point(), GetScrollableControl()->ContentSize());
    
    if (!GetScrollableControl()->UseOverlayScrollBars()) {

        if (need_vertical_scroll_bar) {
            rect.size.width -= GetVerticalScrollBar()->Width();
        }

        if (need_horizontal_scroll_bar) {
            rect.size.height -= GetHorizontalScrollBar()->Height();
        }
    }

    GetScrollableControl()->GetScrollContainerControl()->SetRect(rect);
}

}