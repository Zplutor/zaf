#include <zaf/control/scrollable_control.h>
#include <zaf/control/creation.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/control/self_scrolling_control.h>

namespace zaf {

static const wchar_t* const kScrollBarThicknessPropertyName = L"ScrollBarThickness";

ScrollableControl::ScrollableControl() :
    self_scrolling_control_(nullptr) {

}


ScrollableControl::~ScrollableControl() {

}


void ScrollableControl::Initialize() {

    InitializeVerticalScrollBar(CreateControl<ScrollBar>());
    InitializeHorizontalScrollBar(CreateControl<ScrollBar>());

    scroll_area_control_ = CreateControl<Control>();
    AddChild(scroll_area_control_);

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
    scroll_area_control_->AddChild(scrolled_control_);

    self_scrolling_control_ = dynamic_cast<SelfScrollingControl*>(control.get());
}


void ScrollableControl::Layout(const Rect& previous_rect) {

    LayoutScrollBars();
    LayoutScrollAreaControl();
    AdjustScrolledControlSize();
    AdjustScrollBarValues();
}


void ScrollableControl::LayoutScrollBars() {

    auto content_rect = GetContentRect();
    float scroll_bar_thickness = GetScrollBarThickness();

	Rect vertical_scroll_bar_rect(
        content_rect.position.x + content_rect.size.width - scroll_bar_thickness,
		content_rect.position.y,
        scroll_bar_thickness,
        content_rect.size.height - scroll_bar_thickness
	);
	vertical_scroll_bar_->SetRect(vertical_scroll_bar_rect);

	Rect horizontal_scroll_bar_rect(
		content_rect.position.x,
        content_rect.position.y + content_rect.size.height - scroll_bar_thickness,
        content_rect.size.width - scroll_bar_thickness,
        scroll_bar_thickness
	);
	horizontal_scroll_bar_->SetRect(horizontal_scroll_bar_rect);
}


void ScrollableControl::LayoutScrollAreaControl() {

	auto rect = GetContentRect();
	rect.size.width -= vertical_scroll_bar_->GetWidth();
	rect.size.height -= horizontal_scroll_bar_->GetHeight();

	scroll_area_control_->SetRect(rect);
}


void ScrollableControl::AdjustScrolledControlSize() {

    if (self_scrolling_control_ == nullptr) {
        AdjustGeneralScrolledControlSize();
    }
    else {
        AdjustSelfScrollingControlSize();
    }
}


void ScrollableControl::AdjustGeneralScrolledControlSize() {

    const Size& min_scroll_area_size = scroll_area_control_->GetSize();

    Rect new_rect = scrolled_control_->GetRect();
    new_rect.size = expected_scroll_area_size_;

    if (new_rect.size.width < min_scroll_area_size.width) {
        new_rect.size.width = min_scroll_area_size.width;
        new_rect.position.x = 0;
    }

    if (new_rect.size.height < min_scroll_area_size.height) {
        new_rect.size.height = min_scroll_area_size.height;
        new_rect.position.y = 0;
    }

    scrolled_control_->SetRect(new_rect);
}


void ScrollableControl::AdjustSelfScrollingControlSize() {
    scrolled_control_->SetRect(Rect(Point(), scroll_area_control_->GetSize()));
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
    const Size& content_container_size = scroll_area_control_->GetSize();

    int vertical_scroll_value_range = static_cast<int>(content_size.height - content_container_size.height);
    vertical_scroll_bar_->SetValueRange(0, vertical_scroll_value_range);

    int horizontal_scroll_value_range = static_cast<int>(content_size.width - content_container_size.width);
    horizontal_scroll_bar_->SetValueRange(0, horizontal_scroll_value_range);
}


void ScrollableControl::AdjustScrollBarValuesWithSelfScrollingControl() {

    ScrollInformation vertical_scroll_info;
    self_scrolling_control_->GetVerticalScrollInformation(vertical_scroll_info);
    vertical_scroll_bar_->SetValueRange(vertical_scroll_info.minimum_value, vertical_scroll_info.maximum_value);
    vertical_scroll_bar_->SetValue(vertical_scroll_info.current_value);

    ScrollInformation horizontal_scroll_info;
    self_scrolling_control_->GetHorizontalScrollInformation(horizontal_scroll_info);
    horizontal_scroll_bar_->SetValueRange(horizontal_scroll_info.minimum_value, horizontal_scroll_info.maximum_value);
    horizontal_scroll_bar_->SetValue(horizontal_scroll_info.current_value);
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
        scroll_area_control_->RemoveChild(scrolled_control_);
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