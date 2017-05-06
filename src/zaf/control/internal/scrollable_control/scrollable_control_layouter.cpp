#include <zaf/control/internal/scrollable_control/scrollable_control_layouter.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/scrollable_control.h>

namespace zaf {
namespace internal {

ScrollableControlLayouter::ScrollableControlLayouter(ScrollableControl* scrollable_control) : 
    scrollable_control_(scrollable_control) {

    RegisterScrollBarEvent(GetVerticalScrollBar());
    RegisterScrollBarEvent(GetHorizontalScrollBar());
}


void ScrollableControlLayouter::RegisterScrollBarEvent(const std::shared_ptr<ScrollBar>& scroll_bar) {

    scroll_bar->GetScrollEvent().AddListenerWithTag(
        reinterpret_cast<std::uintptr_t>(this),
        std::bind(&ScrollableControlLayouter::ScrollBarScroll, this, std::placeholders::_1));
}


ScrollableControlLayouter::~ScrollableControlLayouter() {

    UnregisterScrollBarEvent(GetVerticalScrollBar());
    UnregisterScrollBarEvent(GetHorizontalScrollBar());
}


void ScrollableControlLayouter::UnregisterScrollBarEvent(const std::shared_ptr<ScrollBar>& scroll_bar) {
    scroll_bar->GetScrollEvent().RemoveListenersWithTag(reinterpret_cast<std::uintptr_t>(this));
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
        rect.size.width -= GetVerticalScrollBar()->GetWidth();
    }

    if (can_show_horizontal_scroll_bar) {
        rect.size.height -= GetHorizontalScrollBar()->GetHeight();
    }

    GetScrollableControl()->GetScrollContainerControl()->SetRect(rect);
}

}
}