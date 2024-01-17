#pragma once

#include <zaf/control/internal/scrollable_control/scrollable_control_layouter.h>
#include <zaf/control/self_scroll_control.h>
#include <zaf/rx/subscription_host.h>

namespace zaf {
namespace internal {

class SelfScrollingLayouter : public ScrollableControlLayouter, public SubscriptionHost {
public:
    SelfScrollingLayouter(ScrollBox* scrollable_control);
    ~SelfScrollingLayouter();

    void Layout() override;

protected:
    void ScrollBarScroll(const ScrollBarScrollInfo& event_info) override;

private:
    SelfScrollControl* GetSelfScrollingControl() const {
        return GetScrollableControl()->GetSelfScrollingControl();
    }

    void AdjustScrollBarValue(bool is_horizontal);

    void SelfScrollingControlScrollBarChange();
    void SelfScrollingControlScrollValuesChange(
        const SelfScrollControlScrollValuesChangeInfo& event_info);

private:
    bool is_layouting_{};
    bool is_self_scrolling_{};
};

}
}