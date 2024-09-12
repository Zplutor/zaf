#include <zaf/control/internal/scroll_box/general_scroll_layouter.h>
#include <zaf/base/as.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/scroll_box.h>

namespace zaf::internal {

void GeneralScrollLayouter::Layout() {

    bool need_vertical_scroll_bar = false;
    bool need_horizontal_scroll_bar = false;
    auto scroll_content_size = DeterminateScrollContentSize(
        need_vertical_scroll_bar, 
        need_horizontal_scroll_bar);

    LayoutScrollBars(need_vertical_scroll_bar, need_horizontal_scroll_bar);
    LayoutScrollContainerControl(need_vertical_scroll_bar, need_horizontal_scroll_bar);
    AdjustScrollBarValueRanges(scroll_content_size);
    LayoutScrollContentControl(scroll_content_size);
}


Size GeneralScrollLayouter::DeterminateScrollContentSize(
    bool& need_vertical_scroll_bar,
    bool& need_horizontal_scroll_bar) {

    auto determine_if_need_scroll_bar = [this](
        bool allow, 
        float content_length, 
        float container_length) {
    
        if (!allow) {
            return false;
        }

        if (!ScrollBox()->AutoHideScrollBars()) {
            return true;
        } 
        
        return content_length > container_length;
    };

    auto scrollable_control = ScrollBox();
    auto container_size = scrollable_control->ContentSize();

    const auto& content_control = scrollable_control->ScrollContent();
    const auto& content_margin = content_control->Margin();
    const auto& content_size = content_control->Size();

    //First, determine the content height and whether the vertical scroll bar is needed.
    float content_height{};

    auto determine_vertical = [&]() {
    
        if (scrollable_control->AutoScrollContentHeight()) {
            content_height = container_size.height - content_margin.Height();
            content_height = content_control->ApplyHeightLimit(content_height);
        }
        else {
            content_height = content_size.height;
        }

        need_vertical_scroll_bar = determine_if_need_scroll_bar(
            scrollable_control->AllowVerticalScroll(),
            content_height + content_margin.Height(), 
            container_size.height);
    };

    determine_vertical();

    if (need_vertical_scroll_bar && !scrollable_control->UseOverlayScrollBars()) {
        container_size.width -= scrollable_control->VerticalScrollBarThickness();
    }

    //Second, determinate the content width and whether the horizontal scroll bar is needed.
    float content_width{};
    if (scrollable_control->AutoScrollContentWidth()) {
        content_width = container_size.width - content_margin.Width();
        content_width = content_control->ApplyWidthLimit(content_width);
    }
    else {
        content_width = content_size.width;
    }

    need_horizontal_scroll_bar = determine_if_need_scroll_bar(
        scrollable_control->AllowHorizontalScroll(),
        content_width + content_margin.Width(), 
        container_size.width);

    if (need_horizontal_scroll_bar && !scrollable_control->UseOverlayScrollBars()) {

        container_size.height -= scrollable_control->HorizontalScrollBarThickness();

        //Last, revise the content height and whether the vertical scroll bar is needed again, 
        //as the height of container is changed.
        determine_vertical();
    }

    return Size{ content_width, content_height };
}


void GeneralScrollLayouter::AdjustScrollBarValueRanges(const zaf::Size& scroll_content_size) {

    const auto& scroll_container_control = ScrollBox()->GetScrollContainerControl();
    const auto& vertical_scroll_bar = VerticalScrollBar();
    const auto& horizontal_scroll_bar = HorizontalScrollBar();

    const Size& scroll_size = CalculateScrollSize(scroll_content_size);
    const Size& container_size = scroll_container_control->ContentSize();
    auto auto_adjust_large_change = ScrollBox()->AutoScrollBarLargeChange();

    {
        auto update_guard = vertical_scroll_bar->BeginUpdate();

        auto max_value = static_cast<int>(scroll_size.height - container_size.height);
        vertical_scroll_bar->SetIsEnabled(max_value > 0);
        vertical_scroll_bar->SetValueRange(0, max_value);
        vertical_scroll_bar->SetPageSize(static_cast<int>(container_size.height));

        if (auto_adjust_large_change) {
            vertical_scroll_bar->SetLargeChange(static_cast<int>(container_size.height));
        }
    }

    {
        auto update_guard = horizontal_scroll_bar->BeginUpdate();

        auto max_value = static_cast<int>(scroll_size.width - container_size.width);
        horizontal_scroll_bar->SetIsEnabled(max_value > 0);
        horizontal_scroll_bar->SetValueRange(0, max_value);
        horizontal_scroll_bar->SetPageSize(static_cast<int>(container_size.width));

        if (auto_adjust_large_change) {
            horizontal_scroll_bar->SetLargeChange(static_cast<int>(container_size.width));
        }
    }
}


Size GeneralScrollLayouter::CalculateScrollSize(const Size& expected_content_size) const {

    const auto& scroll_content_control = ScrollBox()->ScrollContent();
    const auto& margin = scroll_content_control->Margin();

    auto result = expected_content_size;
    result.width += margin.Width();
    result.height += margin.Height();
    return result;
}


void GeneralScrollLayouter::LayoutScrollContentControl(const zaf::Size& scroll_content_size) {

    const auto& scroll_content_control = ScrollBox()->ScrollContent();
    const auto& margin = scroll_content_control->Margin();

    Rect new_rect;
    new_rect.position.x = margin.left - static_cast<float>(HorizontalScrollBar()->Value());
    new_rect.position.y = margin.top - static_cast<float>(VerticalScrollBar()->Value());
    new_rect.size = scroll_content_size;

    scroll_content_control->SetRect(new_rect);
}


void GeneralScrollLayouter::ScrollBarScroll(const ScrollBarScrollInfo& event_info) {

    const auto& scroll_content_control = ScrollBox()->ScrollContent();
    Rect content_rect = scroll_content_control->Rect();

    auto scroll_bar = As<ScrollBar>(event_info.Source());
    if (scroll_bar->IsHorizontal()) {
        content_rect.position.x = static_cast<float>(-scroll_bar->Value());
    }
    else {
        content_rect.position.y = static_cast<float>(-scroll_bar->Value());
    }

    scroll_content_control->SetRect(content_rect);
}

}