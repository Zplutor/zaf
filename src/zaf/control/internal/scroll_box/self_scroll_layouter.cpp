#include <zaf/control/internal/scroll_box/self_scroll_layouter.h>
#include <zaf/base/as.h>
#include <zaf/base/auto_reset.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/scroll_box.h>
#include <zaf/control/self_scroll_control.h>

namespace zaf {
namespace internal {

SelfScrollLayouter::SelfScrollLayouter(zaf::ScrollBox* scroll_box) :
    ScrollBoxLayouter(scroll_box) {

    auto self_scroll_control = SelfScrollControl();

    Subscriptions() += self_scroll_control->ScrollBarChangeEvent().Subscribe(
        std::bind(&SelfScrollLayouter::SelfScrollingControlScrollBarChange, this));

    Subscriptions() += self_scroll_control->ScrollValuesChangeEvent().Subscribe(
       std::bind(
           &SelfScrollLayouter::SelfScrollingControlScrollValuesChange, 
           this, 
           std::placeholders::_1));
}


SelfScrollLayouter::~SelfScrollLayouter() {

}


void SelfScrollLayouter::Layout() {

    //Not allow reentering.
    ZAF_EXPECT(!is_layouting_);
    auto auto_reset = MakeAutoReset(is_layouting_, true);

    auto scroll_box = ScrollBox();
    auto self_scroll_control = SelfScrollControl();

    bool need_vertical_scroll_bar = 
        scroll_box->AllowVerticalScroll() && 
        self_scroll_control->CanShowVerticalScrollBar();

    bool need_horizontal_scroll_bar = 
        scroll_box->AllowHorizontalScroll() && 
        self_scroll_control->CanShowHorizontalScrollBar();

    LayoutScrollBars(need_vertical_scroll_bar, need_horizontal_scroll_bar);
    LayoutScrollContainerControl(need_vertical_scroll_bar, need_horizontal_scroll_bar);

    auto scroll_content = scroll_box->ScrollContent();
    Rect content_rect{ Point{}, scroll_box->GetScrollContainerControl()->Size() };
    content_rect.Deflate(scroll_content->Margin());
    if (content_rect.size.width < 0) {
        content_rect.size.width = 0;
    }
    if (content_rect.size.height < 0) {
        content_rect.size.height = 0;
    }
    scroll_content->SetRect(content_rect);

    AdjustScrollBarValue(true);
    AdjustScrollBarValue(false);
}


void SelfScrollLayouter::AdjustScrollBarValue(bool is_horizontal) {

    auto scrollable_control = ScrollBox();
    auto self_scrolling_content = SelfScrollControl();

    bool is_enabled{};
    int current_value{};
    int min_value{};
    int max_value{};
    int page_value{};
    std::shared_ptr<ScrollBar> scroll_bar;

    if (is_horizontal) {

        is_enabled = self_scrolling_content->CanEnableHorizontalScrollBar();

        self_scrolling_content->GetHorizontalScrollValues(
            current_value, 
            min_value, 
            max_value, 
            page_value);

        scroll_bar = HorizontalScrollBar();
    }
    else {

        is_enabled = self_scrolling_content->CanEnableVerticalScrollBar();

        self_scrolling_content->GetVerticalScrollValues(
            current_value, 
            min_value, 
            max_value,
            page_value);

        scroll_bar = VerticalScrollBar();
    }

    auto update_guard = scroll_bar->BeginUpdate();
    scroll_bar->SetIsEnabled(is_enabled);
    scroll_bar->SetPageSize(page_value);
    scroll_bar->SetValueRange(min_value, max_value);
    scroll_bar->SetValue(current_value);
}


void SelfScrollLayouter::ScrollBarScroll(const ScrollBarScrollInfo& event_info) {

    if (is_self_scrolling_) {
        return;
    }

    auto scroll_bar = As<ScrollBar>(event_info.Source());

    int value = scroll_bar->Value();
    if (scroll_bar->IsHorizontal()) {
        SelfScrollControl()->HorizontallyScroll(value);
    }
    else {
        SelfScrollControl()->VerticallyScroll(value);
    }
}


void SelfScrollLayouter::SelfScrollingControlScrollBarChange() {
    ScrollBox()->NeedRelayout();
}


void SelfScrollLayouter::SelfScrollingControlScrollValuesChange(
    const SelfScrollControlScrollValuesChangeInfo& event_info) {

    is_self_scrolling_ = true;
    AdjustScrollBarValue(event_info.is_horizontal);
    is_self_scrolling_ = false;
}


}
}