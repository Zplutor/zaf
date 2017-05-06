#pragma once

#include <zaf/control/internal/scrollable_control/scrollable_control_layouter.h>

namespace zaf {
namespace internal {

class GeneralLayouter : public ScrollableControlLayouter {
public:
    GeneralLayouter(ScrollableControl* scrollable_control);

    void Layout() override;

protected:
    void ScrollBarScroll(const std::shared_ptr<ScrollBar>& scroll_bar) override;

private:
    void CanShowScrollBars(bool& can_show_vertical_scroll_bar, bool& can_show_horizontal_scroll_bar);
    void LayoutScrollContentControlSize(bool can_show_vertical_scroll_bar, bool can_show_horizontal_scroll_bar);
    void AdjustScrollBarValuesWithGeneralScrollContentControl();
    Size GetExpectedScrollContentControlSize();
};

}
}