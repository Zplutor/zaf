#include <zaf/control/internal/scroll_box/scroll_box_layouter.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/scroll_box.h>

namespace zaf::internal {

ScrollBoxLayouter::ScrollBoxLayouter(zaf::ScrollBox* scroll_box) : scroll_box_(scroll_box) {

    RegisterScrollBarEvent(VerticalScrollBar(), false);
    RegisterScrollBarEvent(HorizontalScrollBar(), true);
}


ScrollBoxLayouter::~ScrollBoxLayouter() {

    UnregisterScrollBarEvent(true);
    UnregisterScrollBarEvent(false);
}


void ScrollBoxLayouter::ScrollBarChange(
    bool is_horizontal, 
    const std::shared_ptr<ScrollBar>& previous_scroll_bar) {

    UnregisterScrollBarEvent(is_horizontal);

    auto new_scroll_bar = is_horizontal ? HorizontalScrollBar() : VerticalScrollBar();
    RegisterScrollBarEvent(new_scroll_bar, is_horizontal);
}


void ScrollBoxLayouter::RegisterScrollBarEvent(
    const std::shared_ptr<ScrollBar>& scroll_bar, 
    bool is_horizontal) {

    auto& subscription =
        is_horizontal ?
        horizontal_scroll_bar_subscription_ :
        vertical_scroll_bar_subscription_;

    subscription = scroll_bar->ScrollEvent().Subscribe(
        std::bind(&ScrollBoxLayouter::ScrollBarScroll, this, std::placeholders::_1));
}


void ScrollBoxLayouter::UnregisterScrollBarEvent(bool is_horizontal) {

    auto& subscription = 
        is_horizontal ? 
        horizontal_scroll_bar_subscription_ : 
        vertical_scroll_bar_subscription_;

    subscription.Unsubscribe();
}


void ScrollBoxLayouter::LayoutScrollBars(
    bool need_vertical_scroll_bar, 
    bool need_horizontal_scroll_bar) {

    auto content_size = ScrollBox()->ContentSize();
    float vertical_scroll_bar_thickness = ScrollBox()->VerticalScrollBarThickness();
    float horizontal_scroll_bar_thickness = ScrollBox()->HorizontalScrollBarThickness();

    //Layout vertical scroll bar
    VerticalScrollBar()->SetIsVisible(need_vertical_scroll_bar);
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
        VerticalScrollBar()->SetRect(vertical_scroll_bar_rect);
    }

    //Layout horizontal scroll bar
    HorizontalScrollBar()->SetIsVisible(need_horizontal_scroll_bar);
    if (need_horizontal_scroll_bar) {

        Rect horizontal_scroll_bar_rect(
            0,
            content_size.height - horizontal_scroll_bar_thickness,
            content_size.width - (need_vertical_scroll_bar ? vertical_scroll_bar_thickness : 0),
            horizontal_scroll_bar_thickness
        );
        HorizontalScrollBar()->SetRect(horizontal_scroll_bar_rect);
    }

    //Layout scroll bar corner
    const auto& corner = ScrollBox()->ScrollBarCorner();
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


void ScrollBoxLayouter::LayoutScrollContainerControl(
    bool need_vertical_scroll_bar, 
    bool need_horizontal_scroll_bar) {

    Rect rect(Point(), ScrollBox()->ContentSize());
    
    if (!ScrollBox()->UseOverlayScrollBars()) {

        if (need_vertical_scroll_bar) {
            rect.size.width -= VerticalScrollBar()->Width();
        }

        if (need_horizontal_scroll_bar) {
            rect.size.height -= HorizontalScrollBar()->Height();
        }
    }

    ScrollBox()->GetScrollContainerControl()->SetRect(rect);
}

}