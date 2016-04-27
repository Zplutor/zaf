#include <zaf/control/scrollable_control.h>
#include <zaf/control/creation.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/self_scrolling_control.h>

namespace zaf {

static const wchar_t* const kAllowHorizontalScrollPropertyName = L"AllowHorizontalScroll";
static const wchar_t* const kAllowVerticalScrollPropertyName = L"AllowVerticalScroll";
static const wchar_t* const kAutoHideScrollBarsPropertyName = L"AutoHideScrollBars";
static const wchar_t* const kScrollBarThicknessPropertyName = L"ScrollBarThickness";

ScrollableControl::ScrollableControl() :
    self_scrolling_control_(nullptr) {

}


ScrollableControl::~ScrollableControl() {

}


void ScrollableControl::Initialize() {

    InitializeVerticalScrollBar(CreateControl<ScrollBar>());
    InitializeHorizontalScrollBar(CreateControl<ScrollBar>());

    scroll_container_control_ = CreateControl<Control>();
    AddChild(scroll_container_control_);

    InitializeScrolledControl(CreateControl<Control>());
}


void ScrollableControl::InitializeVerticalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar) {

    vertical_scroll_bar_ = scroll_bar;
    InitializeScrollBar(vertical_scroll_bar_);
}


void ScrollableControl::InitializeHorizontalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar) {

    horizontal_scroll_bar_ = scroll_bar;
    horizontal_scroll_bar_->SetIsHorizontal(true);
    InitializeScrollBar(horizontal_scroll_bar_);
}


void ScrollableControl::InitializeScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar) {

    AddChild(scroll_bar);
    scroll_bar->GetScrollEvent().AddListener(std::bind(&ScrollableControl::ScrollBarScroll, this, std::placeholders::_1));
}


void ScrollableControl::InitializeScrolledControl(const std::shared_ptr<Control>& control) {

    scrolled_control_ = control;
    scroll_container_control_->AddChild(scrolled_control_);

    self_scrolling_control_ = dynamic_cast<SelfScrollingControl*>(control.get());
}


void ScrollableControl::Layout(const Rect& previous_rect) {

    bool can_show_vertical_scroll_bar = false;
    bool can_show_horizontal_scroll_bar = false;
    CanShowScrollBars(can_show_vertical_scroll_bar, can_show_horizontal_scroll_bar);

    LayoutScrollBars(can_show_vertical_scroll_bar, can_show_horizontal_scroll_bar);
    LayoutScrollContainerControl(can_show_vertical_scroll_bar, can_show_horizontal_scroll_bar);

    AdjustScrolledControlSize(can_show_vertical_scroll_bar, can_show_horizontal_scroll_bar);
    AdjustScrollBarValues();
}


void ScrollableControl::CanShowScrollBars(bool& can_show_vertical_scroll_bar, bool& can_show_horizontal_scroll_bar) const {

    if (self_scrolling_control_ == nullptr) {
        CanShowScrollBarsWithGeneralScrolledControl(can_show_vertical_scroll_bar, can_show_horizontal_scroll_bar);
    }
    else {
        CanShowScrollBarsWithSelScrollingControl(can_show_vertical_scroll_bar, can_show_horizontal_scroll_bar);
    }
}


void ScrollableControl::CanShowScrollBarsWithGeneralScrolledControl(bool& can_show_vertical_scroll_bar, bool& can_show_horizontal_scroll_bar) const {

    Rect content_rect = GetContentRect();
    float content_width = content_rect.size.width;
    float content_height = content_rect.size.height;

    bool allow_vertical_scroll = AllowVerticalScroll();
    bool allow_horizontal_scroll = AllowHorizontalScroll();

    bool auto_hide_scroll_bars = AutoHideScrollBars();

    auto determine = [auto_hide_scroll_bars](bool allow, float expected, float actual) {
        return allow && (!auto_hide_scroll_bars || (expected > actual));
    };

    can_show_vertical_scroll_bar = determine(allow_vertical_scroll, expected_scroll_area_size_.height, content_height);
    can_show_horizontal_scroll_bar = determine(allow_horizontal_scroll, expected_scroll_area_size_.width, content_width);

    float scroll_bar_thickness = GetScrollBarThickness();

    if (can_show_vertical_scroll_bar) {
        can_show_horizontal_scroll_bar = determine(
            allow_horizontal_scroll,
            expected_scroll_area_size_.width,
            content_width - scroll_bar_thickness
        );
    }

    if (can_show_horizontal_scroll_bar) {
        can_show_vertical_scroll_bar = determine(
            allow_vertical_scroll,
            expected_scroll_area_size_.height,
            content_height - scroll_bar_thickness
        );
    }
}


void ScrollableControl::CanShowScrollBarsWithSelScrollingControl(bool& can_show_vertical_scroll_bar, bool& can_show_horizontal_scroll_bar) const {


}


void ScrollableControl::LayoutScrollBars(bool can_show_vertical_scroll_bar, bool can_show_horizontal_scroll_bar) {

    vertical_scroll_bar_->SetIsVisible(can_show_vertical_scroll_bar);
    horizontal_scroll_bar_->SetIsVisible(can_show_horizontal_scroll_bar);

    auto content_rect = GetContentRect();
    float scroll_bar_thickness = GetScrollBarThickness();

    if (can_show_vertical_scroll_bar) {

	    Rect vertical_scroll_bar_rect(
            content_rect.position.x + content_rect.size.width - scroll_bar_thickness,
		    content_rect.position.y,
            scroll_bar_thickness,
            content_rect.size.height - scroll_bar_thickness
	    );
	    vertical_scroll_bar_->SetRect(vertical_scroll_bar_rect);
    }

    if (can_show_horizontal_scroll_bar) {

	    Rect horizontal_scroll_bar_rect(
		    content_rect.position.x,
            content_rect.position.y + content_rect.size.height - scroll_bar_thickness,
            content_rect.size.width - scroll_bar_thickness,
            scroll_bar_thickness
	    );
	    horizontal_scroll_bar_->SetRect(horizontal_scroll_bar_rect);
    }
}


void ScrollableControl::LayoutScrollContainerControl(bool can_show_vertical_scroll_bar, bool can_show_horizontal_scroll_bar) {

	auto rect = GetContentRect();

    if (can_show_vertical_scroll_bar) {
	    rect.size.width -= vertical_scroll_bar_->GetWidth();
    }

    if (can_show_horizontal_scroll_bar) {
	    rect.size.height -= horizontal_scroll_bar_->GetHeight();
    }

	scroll_container_control_->SetRect(rect);
}


void ScrollableControl::AdjustScrolledControlSize(bool can_show_vertical_scroll_bar, bool can_show_horizontal_scroll_bar) {

    if (self_scrolling_control_ == nullptr) {
        AdjustGeneralScrolledControlSize(can_show_vertical_scroll_bar, can_show_horizontal_scroll_bar);
    }
    else {
        AdjustSelfScrollingControlSize();
    }
}


void ScrollableControl::AdjustGeneralScrolledControlSize(bool can_show_vertical_scroll_bar, bool can_show_horizontal_scroll_bar) {

    const Size& min_size = scroll_container_control_->GetSize();

    Rect new_rect = scrolled_control_->GetRect();
    new_rect.size = expected_scroll_area_size_;

    if (! can_show_horizontal_scroll_bar || new_rect.size.width < min_size.width) {
        new_rect.size.width = min_size.width;
        new_rect.position.x = 0;
    }

    if (! can_show_vertical_scroll_bar || new_rect.size.height < min_size.height) {
        new_rect.size.height = min_size.height;
        new_rect.position.y = 0;
    }

    scrolled_control_->SetRect(new_rect);
}


void ScrollableControl::AdjustSelfScrollingControlSize() {
    scrolled_control_->SetRect(Rect(Point(), scroll_container_control_->GetSize()));
}


void ScrollableControl::AdjustScrollBarValues() {

    if (self_scrolling_control_ == nullptr) {
        AdjustScrollBarValuesWithGeneralScrolledControl();
    }
    else {
        AdjustScrollBarValuesWithSelfScrollingControl();
    }
}


void ScrollableControl::AdjustScrollBarValuesWithGeneralScrolledControl() {

    const Size& content_size = scrolled_control_->GetSize();
    const Size& content_container_size = scroll_container_control_->GetSize();

    int vertical_scroll_value_range = static_cast<int>(content_size.height - content_container_size.height);
    vertical_scroll_bar_->SetValueRange(0, vertical_scroll_value_range);
    if (vertical_scroll_value_range == 0) {
        vertical_scroll_bar_->SetIsEnabled(false);
    }

    int horizontal_scroll_value_range = static_cast<int>(content_size.width - content_container_size.width);
    horizontal_scroll_bar_->SetValueRange(0, horizontal_scroll_value_range);
    if (horizontal_scroll_value_range == 0) {
        horizontal_scroll_bar_->SetIsEnabled(false);
    }
}


void ScrollableControl::AdjustScrollBarValuesWithSelfScrollingControl() {

    int current_value = 0;
    int min_value = 0;
    int max_value = 0;

    self_scrolling_control_->GetVerticalScrollValues(current_value, min_value, max_value);
    vertical_scroll_bar_->SetValueRange(min_value, max_value);
    vertical_scroll_bar_->SetValue(current_value);

    self_scrolling_control_->GetHorizontalScrollValues(current_value, min_value, max_value);
    horizontal_scroll_bar_->SetValueRange(min_value, max_value);
    horizontal_scroll_bar_->SetValue(current_value);
}


bool ScrollableControl::AllowVerticalScroll() const {

    auto allow_scroll = GetPropertyMap().TryGetProperty<bool>(kAllowVerticalScrollPropertyName);
    if (allow_scroll != nullptr) {
        return *allow_scroll;
    }
    else {
        return true;
    }
}


void ScrollableControl::SetAllowVerticalScroll(bool allow_scroll) {

    GetPropertyMap().SetProperty(kAllowVerticalScrollPropertyName, allow_scroll);
    NeedRelayout();
}


bool ScrollableControl::AllowHorizontalScroll() const {

    auto allow_scroll = GetPropertyMap().TryGetProperty<bool>(kAllowHorizontalScrollPropertyName);
    if (allow_scroll != nullptr) {
        return *allow_scroll;
    }
    else {
        return true;
    }
}


void ScrollableControl::SetAllowHorizontalScroll(bool allow_scroll) {

    GetPropertyMap().SetProperty(kAllowHorizontalScrollPropertyName, allow_scroll);
    NeedRelayout();
}


bool ScrollableControl::AutoHideScrollBars() const {

    auto always_show = GetPropertyMap().TryGetProperty<bool>(kAutoHideScrollBarsPropertyName);
    if (always_show != nullptr) {
        return *always_show;
    }
    else {
        return false;
    }
}


void ScrollableControl::SetAutoHideScrollBars(bool always_show) {

    GetPropertyMap().SetProperty(kAutoHideScrollBarsPropertyName, always_show);
    NeedRelayout();
}


void ScrollableControl::SetVerticalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar) {

    if (vertical_scroll_bar_ != nullptr) {
        RemoveChild(vertical_scroll_bar_);
    }

    InitializeVerticalScrollBar(scroll_bar);
    NeedRelayout();
}


void ScrollableControl::SetHorizontalScrollBar(const std::shared_ptr<ScrollBar>& scroll_bar) {

    if (horizontal_scroll_bar_ != nullptr) {
        RemoveChild(horizontal_scroll_bar_);
    }

    InitializeHorizontalScrollBar(scroll_bar);
    NeedRelayout();
}


void ScrollableControl::SetScrolledControl(const std::shared_ptr<Control>& control) {

    if (scrolled_control_ != nullptr) {
        scroll_container_control_->RemoveChild(scrolled_control_);
    }

    InitializeScrolledControl(control);
    NeedRelayout();
}


float ScrollableControl::GetScrollBarThickness() const {

    auto thickness = GetPropertyMap().TryGetProperty<float>(kScrollBarThicknessPropertyName);
    if (thickness != nullptr) {
        return *thickness;
    }
    else {
        return 12;
    }
}


void ScrollableControl::SetScrollBarThickness(float thickness) {

    GetPropertyMap().SetProperty(kScrollBarThicknessPropertyName, thickness);
    NeedRelayout();
}


void ScrollableControl::SetScrollAreaSize(const Size& size) {

    expected_scroll_area_size_ = size;
    NeedRelayout();
}


void ScrollableControl::MouseWheel(const Point& position, bool is_horizontal, int distance, WPARAM wParam, LPARAM lParam) {

    if (is_horizontal) {
        horizontal_scroll_bar_->Wheel(distance);
    }
    else {
        vertical_scroll_bar_->Wheel(distance);
    }
}


void ScrollableControl::ScrollBarScroll(const std::shared_ptr<ScrollBar>& scroll_bar) {

	Rect content_rect = scrolled_control_->GetRect();

	if (scroll_bar == vertical_scroll_bar_) {
		content_rect.position.y = static_cast<float>(-scroll_bar->GetValue());
	}
	else if (scroll_bar == horizontal_scroll_bar_) {
		content_rect.position.x = static_cast<float>(-scroll_bar->GetValue());
	}

    scrolled_control_->SetRect(content_rect);
}


}