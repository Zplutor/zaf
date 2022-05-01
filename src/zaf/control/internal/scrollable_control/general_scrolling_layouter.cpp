#include <zaf/control/internal/scrollable_control/general_scrolling_layouter.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/scrollable_control.h>

namespace zaf::internal {

GeneralScrollingLayouter::GeneralScrollingLayouter(ScrollableControl* scrollable_control) : 
    ScrollableControlLayouter(scrollable_control) {

}


void GeneralScrollingLayouter::Layout() {

    bool need_vertical_scroll_bar = false;
    bool need_horizontal_scroll_bar = false;
    auto scroll_content_size = DeterminateScrollContentSize(
        need_vertical_scroll_bar, 
        need_horizontal_scroll_bar);

    LayoutScrollBars(need_vertical_scroll_bar, need_horizontal_scroll_bar);
    LayoutScrollContainerControl(need_vertical_scroll_bar, need_horizontal_scroll_bar);
    LayoutScrollContentControl(scroll_content_size);
    AdjustScrollBarValueRanges();
}


Size GeneralScrollingLayouter::DeterminateScrollContentSize(
    bool& need_vertical_scroll_bar,
    bool& need_horizontal_scroll_bar) {

    auto determinate_if_need_scroll_bar = [this](
        bool allow, 
        float content_length, 
        float container_length) {
    
        if (!allow) {
            return false;
        }

        if (!GetScrollableControl()->AutoHideScrollBars()) {
            return true;
        } 
        
        return content_length > container_length;
    };

    auto scrollable_control = GetScrollableControl();
    auto container_size = scrollable_control->ContentSize();

    const auto& content_control = scrollable_control->ScrollContent();
    const auto& content_margin = content_control->Margin();
    const auto& content_size = content_control->Size();

    //First, determinate content height and if need vertical scroll bar.
    float content_height{};

    auto determinate_vertical = [&]() {
    
        if (scrollable_control->AutoScrollContentHeight()) {
            content_height = container_size.height - content_margin.Height();
            content_height = content_control->ApplyHeightLimit(content_height);
        }
        else {
            content_height = content_size.height;
        }

        need_vertical_scroll_bar = determinate_if_need_scroll_bar(
            scrollable_control->AllowVerticalScroll(),
            content_height + content_margin.Height(), 
            container_size.height);
    };

    determinate_vertical();

    if (need_vertical_scroll_bar) {
        container_size.width -= scrollable_control->ScrollBarThickness();
    }

    //Second, determinate content width and if need horizontal scroll bar.
    float content_width{};
    if (scrollable_control->AutoScrollContentWidth()) {
        content_width = container_size.width - content_margin.Width();
        content_width = content_control->ApplyWidthLimit(content_width);
    }
    else {
        content_width = content_size.width;
    }

    need_horizontal_scroll_bar = determinate_if_need_scroll_bar(
        scrollable_control->AllowHorizontalScroll(),
        content_width + content_margin.Width(), 
        container_size.width);

    if (need_horizontal_scroll_bar) {

        container_size.height -= scrollable_control->ScrollBarThickness();

        //Last, revise content height and if need vertical scroll bar again, because the height of 
        // container is changed.
        determinate_vertical();
    }

    return Size{ content_width, content_height };
}


void GeneralScrollingLayouter::LayoutScrollContentControl(const zaf::Size& scroll_content_size) {

    const auto& scroll_content_control = GetScrollableControl()->ScrollContent();
    const auto& margin = scroll_content_control->Margin();

    Rect new_rect;
    new_rect.position.x = margin.left;
    new_rect.position.y = margin.top;
    new_rect.size = scroll_content_size;

    scroll_content_control->SetRect(new_rect);
}


void GeneralScrollingLayouter::AdjustScrollBarValueRanges() {

    const auto& scroll_container_control = GetScrollableControl()->GetScrollContainerControl();
    const auto& vertical_scroll_bar = GetVerticalScrollBar();
    const auto& horizontal_scroll_bar = GetHorizontalScrollBar();

    const Size& scroll_size = GetScrollSize();
    const Size& container_size = scroll_container_control->ContentSize();
    auto auto_adjust_large_change = GetScrollableControl()->AutoScrollBarLargeChange();

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


Size GeneralScrollingLayouter::GetScrollSize() {

    const auto& scroll_content_control = GetScrollableControl()->ScrollContent();
    const auto& margin = scroll_content_control->Margin();

    auto result = scroll_content_control->Size();
    result.width += margin.left + margin.right;
    result.height += margin.top + margin.bottom;
    return result;
}


void GeneralScrollingLayouter::ScrollBarScroll(const ScrollBarScrollInfo& event_info) {

    const auto& scroll_content_control = GetScrollableControl()->ScrollContent();

    Rect content_rect = scroll_content_control->Rect();

    if (event_info.ScrollBar()->IsHorizontal()) {
        content_rect.position.x = static_cast<float>(-event_info.ScrollBar()->GetValue());
    }
    else {
        content_rect.position.y = static_cast<float>(-event_info.ScrollBar()->GetValue());
    }

    scroll_content_control->SetRect(content_rect);
}

}