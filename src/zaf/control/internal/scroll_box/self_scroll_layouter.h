#pragma once

#include <zaf/control/internal/scroll_box/scroll_box_layouter.h>
#include <zaf/control/self_scroll_control.h>
#include <zaf/rx/subscription_host.h>

namespace zaf {
namespace internal {

class SelfScrollLayouter : public ScrollBoxLayouter, public SubscriptionHost {
public:
    explicit SelfScrollLayouter(zaf::ScrollBox* scroll_box);
    ~SelfScrollLayouter();

    void Layout() override;

protected:
    void ScrollBarScroll(const ScrollBarScrollInfo& event_info) override;

private:
    SelfScrollControl* SelfScrollControl() const {
        return ScrollBox()->GetSelfScrollingControl();
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