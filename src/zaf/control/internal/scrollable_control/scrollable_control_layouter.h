#pragma once

#include <memory>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/scrollable_control.h>

namespace zaf::internal {

class ScrollableControlLayouter {
public:
    ScrollableControlLayouter(ScrollableControl* scrollable_control);
    virtual ~ScrollableControlLayouter();

    ScrollableControlLayouter(const ScrollableControlLayouter&) = delete;
    ScrollableControlLayouter& operator=(const ScrollableControlLayouter&) = delete;

    void ScrollBarChange(bool is_horizontal, const std::shared_ptr<ScrollBar>& previous_scroll_bar);

    virtual void Layout() = 0;

protected:
    ScrollableControl* GetScrollableControl() const {
        return scrollable_control_;
    }

    const std::shared_ptr<ScrollBar>& GetVerticalScrollBar() const {
        return GetScrollableControl()->VerticalScrollBar();
    }

    const std::shared_ptr<ScrollBar>& GetHorizontalScrollBar() const {
        return GetScrollableControl()->HorizontalScrollBar();
    }

    void LayoutScrollBars(bool need_vertical_scroll_bar, bool need_horizontal_scroll_bar);
    void LayoutScrollContainerControl(
        bool need_vertical_scroll_bar,
        bool need_horizontal_scroll_bar);

    virtual void ScrollBarScroll(const ScrollBarScrollInfo& event_info) = 0;

private:
    void RegisterScrollBarEvent(const std::shared_ptr<ScrollBar>& scroll_bar, bool is_horizontal);
    void UnregisterScrollBarEvent(bool is_horizontal);

private:
    ScrollableControl* scrollable_control_;

    Subscription horizontal_scroll_bar_subscription_;
    Subscription vertical_scroll_bar_subscription_;
};

}