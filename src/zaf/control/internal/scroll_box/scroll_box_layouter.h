#pragma once

#include <memory>
#include <zaf/base/non_copyable.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/scroll_box.h>

namespace zaf::internal {

class ScrollBoxLayouter : NonCopyableNonMovable {
public:
    explicit ScrollBoxLayouter(zaf::ScrollBox* scroll_box);
    virtual ~ScrollBoxLayouter();

    void ScrollBarChange(
        bool is_horizontal, 
        const std::shared_ptr<ScrollBar>& previous_scroll_bar);

    virtual void Layout() = 0;

protected:
    zaf::ScrollBox* ScrollBox() const {
        return scroll_box_;
    }

    const std::shared_ptr<ScrollBar>& VerticalScrollBar() const {
        return ScrollBox()->VerticalScrollBar();
    }

    const std::shared_ptr<ScrollBar>& HorizontalScrollBar() const {
        return ScrollBox()->HorizontalScrollBar();
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
    zaf::ScrollBox* scroll_box_{};

    Subscription horizontal_scroll_bar_subscription_;
    Subscription vertical_scroll_bar_subscription_;
};

}