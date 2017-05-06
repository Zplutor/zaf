#pragma once

#include <zaf/control/internal/scrollable_control/scrollable_control_layouter.h>

namespace zaf {
namespace internal {

class SelfScrollingLayouter : public ScrollableControlLayouter {
public:
    SelfScrollingLayouter(ScrollableControl* scrollable_control);
    ~SelfScrollingLayouter();

    void Layout() override;

protected:
    void ScrollBarScroll(const std::shared_ptr<ScrollBar>& scroll_bar) override;

private:
    SelfScrollingControl* GetSelfScrollingControl() const {
        return GetScrollableControl()->GetSelfScrollingControl();
    }

    void AdjustScrollBarValue(bool is_horizontal);

    void SelfScrollingControlScrollBarChange();
    void SelfScrollingControlScrollValuesChange(bool is_horizontal);

private:
    bool is_self_scrolling_ = false;
};

}
}