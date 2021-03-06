#include <zaf/control/internal/scrollable_control/general_layouter.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/scrollable_control.h>

namespace zaf {
namespace internal {

GeneralLayouter::GeneralLayouter(ScrollableControl* scrollable_control) : 
    ScrollableControlLayouter(scrollable_control) {

}


void GeneralLayouter::Layout() {

    bool can_show_vertical_scroll_bar = false;
    bool can_show_horizontal_scroll_bar = false;
    CanShowScrollBars(can_show_vertical_scroll_bar, can_show_horizontal_scroll_bar);

    LayoutScrollBars(can_show_vertical_scroll_bar, can_show_horizontal_scroll_bar);
    LayoutScrollContainerControl(can_show_vertical_scroll_bar, can_show_horizontal_scroll_bar);
    LayoutScrollContentControlSize(can_show_vertical_scroll_bar, can_show_horizontal_scroll_bar);
    AdjustScrollBarValueRanges();
}


void GeneralLayouter::CanShowScrollBars(
    bool& can_show_vertical_scroll_bar,
    bool& can_show_horizontal_scroll_bar) {

    Rect content_rect = GetScrollableControl()->GetContentRect();
    float content_width = content_rect.size.width;
    float content_height = content_rect.size.height;

    Size expected_scroll_content_control_size = GetExpectedScrollContentControlSize();

    bool allow_vertical_scroll = GetScrollableControl()->AllowVerticalScroll();
    bool allow_horizontal_scroll = GetScrollableControl()->AllowHorizontalScroll();

    bool auto_hide_scroll_bars = GetScrollableControl()->AutoHideScrollBars();

    auto determine = [auto_hide_scroll_bars](bool allow, float expected, float actual) {
        return allow && (!auto_hide_scroll_bars || (expected > actual));
    };

    can_show_vertical_scroll_bar = determine(
        allow_vertical_scroll, 
        expected_scroll_content_control_size.height,
        content_height);

    can_show_horizontal_scroll_bar = determine(
        allow_horizontal_scroll, 
        expected_scroll_content_control_size.width,
        content_width);

    float scroll_bar_thickness = GetScrollableControl()->GetScrollBarThickness();

    if (can_show_vertical_scroll_bar) {
        can_show_horizontal_scroll_bar = determine(
            allow_horizontal_scroll,
            expected_scroll_content_control_size.width,
            content_width - scroll_bar_thickness
            );
    }

    if (can_show_horizontal_scroll_bar) {
        can_show_vertical_scroll_bar = determine(
            allow_vertical_scroll,
            expected_scroll_content_control_size.height,
            content_height - scroll_bar_thickness
            );
    }
}


void GeneralLayouter::LayoutScrollContentControlSize(
    bool can_show_vertical_scroll_bar, 
    bool can_show_horizontal_scroll_bar) {

    const auto& scroll_content_control = GetScrollableControl()->GetScrollContentControl();

    Rect new_rect = scroll_content_control->GetRect();
    new_rect.size = GetExpectedScrollContentControlSize();

    const Size& min_size = GetScrollableControl()->GetScrollContainerControl()->GetSize();

    if (!can_show_horizontal_scroll_bar || new_rect.size.width < min_size.width) {
        new_rect.size.width = min_size.width;
        new_rect.position.x = 0;
    }

    if (!can_show_vertical_scroll_bar || new_rect.size.height < min_size.height) {
        new_rect.size.height = min_size.height;
        new_rect.position.y = 0;
    }

    scroll_content_control->SetRect(new_rect);
}


Size GeneralLayouter::GetExpectedScrollContentControlSize() {

    const auto& scroll_content_control = GetScrollableControl()->GetScrollContentControl();
    const auto& padding = scroll_content_control->GetPadding();
    const auto& border = scroll_content_control->GetBorder();

    Size size = GetScrollableControl()->GetExpectedScrollContentSize();
    size.width += padding.left + padding.right + border.left + border.right;
    size.height += padding.top + padding.bottom + border.top + border.bottom;
    return size;
}


void GeneralLayouter::AdjustScrollBarValueRanges() {

    const auto& scroll_content_control = GetScrollableControl()->GetScrollContentControl();
    const auto& scroll_container_control = GetScrollableControl()->GetScrollContainerControl();
    const auto& vertical_scroll_bar = GetVerticalScrollBar();
    const auto& horizontal_scroll_bar = GetHorizontalScrollBar();

    const Size& content_size = scroll_content_control->GetSize();
    const Size& container_size = scroll_container_control->GetContentSize();
    auto auto_adjust_large_change = GetScrollableControl()->AutoAdjustScrollBarLargeChange();

    {
        auto update_guard = vertical_scroll_bar->BeginUpdate();

        auto max_value = static_cast<int>(content_size.height - container_size.height);
        vertical_scroll_bar->SetIsEnabled(max_value > 0);
        vertical_scroll_bar->SetValueRange(0, max_value);
        vertical_scroll_bar->SetPageSize(static_cast<int>(container_size.height));

        if (auto_adjust_large_change) {
            vertical_scroll_bar->SetLargeChange(static_cast<int>(container_size.height));
        }
    }

    {
        auto update_guard = horizontal_scroll_bar->BeginUpdate();

        auto max_value = static_cast<int>(content_size.width - container_size.width);
        horizontal_scroll_bar->SetIsEnabled(max_value > 0);
        horizontal_scroll_bar->SetValueRange(0, max_value);
        horizontal_scroll_bar->SetPageSize(static_cast<int>(container_size.width));

        if (auto_adjust_large_change) {
            horizontal_scroll_bar->SetLargeChange(static_cast<int>(container_size.width));
        }
    }
}


void GeneralLayouter::ScrollBarScroll(const ScrollBarScrollInfo& event_info) {

    const auto& scroll_content_control = GetScrollableControl()->GetScrollContentControl();

    Rect content_rect = scroll_content_control->GetRect();

    if (event_info.scroll_bar->IsHorizontal()) {
        content_rect.position.x = static_cast<float>(-event_info.scroll_bar->GetValue());
    }
    else {
        content_rect.position.y = static_cast<float>(-event_info.scroll_bar->GetValue());
    }

    scroll_content_control->SetRect(content_rect);
}

}
}