#pragma once

#include <zaf/control/internal/scroll_box/scroll_box_layouter.h>

namespace zaf::internal {

class GeneralScrollLayouter : public ScrollBoxLayouter {
public:
    using ScrollBoxLayouter::ScrollBoxLayouter;

    void Layout() override;

protected:
    void ScrollBarScroll(const ScrollBarScrollInfo& event_info) override;

private:
    Size DeterminateScrollContentSize(
        bool& need_vertical_scroll_bar,
        bool& need_horizontal_scroll_bar);

    void AdjustScrollBarValueRanges(const Size& scroll_content_size);
    Size CalculateScrollSize(const Size& expected_content_size) const;

    void LayoutScrollContentControl(const Size& scroll_content_size);
};

}