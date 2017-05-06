#pragma once

#include <memory>
#include <zaf/control/scrollable_control.h>

namespace zaf {
namespace internal {

class ScrollableControlLayouter {
public:
    ScrollableControlLayouter(ScrollableControl* scrollable_control);
    virtual ~ScrollableControlLayouter();

    virtual void Layout() = 0;

    ScrollableControlLayouter(const ScrollableControlLayouter&) = delete;
    ScrollableControlLayouter& operator=(const ScrollableControlLayouter&) = delete;

protected:
    ScrollableControl* GetScrollableControl() const {
        return scrollable_control_;
    }

    const std::shared_ptr<ScrollBar>& GetVerticalScrollBar() const {
        return GetScrollableControl()->GetVerticalScrollBar();
    }

    const std::shared_ptr<ScrollBar>& GetHorizontalScrollBar() const {
        return GetScrollableControl()->GetHorizontalScrollBar();
    }

    void LayoutScrollBars(bool can_show_vertical_scroll_bar, bool can_show_horizontal_scroll_bar);
    void LayoutScrollContainerControl(bool can_show_vertical_scroll_bar, bool can_show_horizontal_scroll_bar);

    virtual void ScrollBarScroll(const std::shared_ptr<ScrollBar>& scroll_bar) = 0;

private:
    void RegisterScrollBarEvent(const std::shared_ptr<ScrollBar>& scroll_bar);
    void UnregisterScrollBarEvent(const std::shared_ptr<ScrollBar>& scroll_bar);

private:
    ScrollableControl* scrollable_control_;
};

}
}