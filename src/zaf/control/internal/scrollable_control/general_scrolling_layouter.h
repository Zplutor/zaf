#pragma once

#include <zaf/control/internal/scrollable_control/scrollable_control_layouter.h>

namespace zaf::internal {

class GeneralScrollingLayouter : public ScrollableControlLayouter {
public:
    GeneralScrollingLayouter(ScrollableControl* scrollable_control);

    void Layout() override;

protected:
    void ScrollBarScroll(const ScrollBarScrollInfo& event_info) override;

private:
    Size DeterminateScrollContentSize(
        bool& need_vertical_scroll_bar,
        bool& need_horizontal_scroll_bar);

    void LayoutScrollContentControl(const zaf::Size& scroll_content_size);

    void AdjustScrollBarValueRanges();
    Size GetScrollSize();
};

}