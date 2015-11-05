#include <zaf/scrollable_control.h>
#include <zaf/scroll_bar.h>
#include <zaf/internal/theme/painter.h>
#include <zaf/internal/theme/theme.h>

namespace zaf {

static const float kScrollBarThickness = 17;

ScrollableControl::ScrollableControl() :
	content_control_(std::make_shared<Control>()),
	content_container_control_(std::make_shared<Control>()),
	vertical_scroll_bar_(std::make_shared<ScrollBar>()),
	horizontal_scroll_bar_(std::make_shared<ScrollBar>()) {

	content_control_->AddAnchor(Anchor::Left);
	content_control_->AddAnchor(Anchor::Top);

	content_container_control_->AddChild(content_control_);

	vertical_scroll_bar_->SetArrowLength(kScrollBarThickness);
	vertical_scroll_bar_->SetRect(Rect(0, 0, kScrollBarThickness, 0));

	horizontal_scroll_bar_->SetIsHorizontal(true);
	horizontal_scroll_bar_->SetArrowLength(kScrollBarThickness);
	horizontal_scroll_bar_->SetRect(Rect(0, 0, 0, kScrollBarThickness));
}


ScrollableControl::~ScrollableControl() {

}


void ScrollableControl::Initialize() {

	AddChild(content_container_control_);

	auto scroll_event_listener = std::bind(&ScrollableControl::ScrollBarScroll, this, std::placeholders::_1);

	vertical_scroll_bar_->OnScroll.AddListenerWithTag(scroll_event_listener, this);
	AddChild(vertical_scroll_bar_);

	horizontal_scroll_bar_->OnScroll.AddListenerWithTag(scroll_event_listener, this);
	AddChild(horizontal_scroll_bar_);
}


void ScrollableControl::SetContentSize(const Size& size) {

	content_size_ = size;
	NeedRelayout();
}


void ScrollableControl::Layout(const Rect& previous_rect) {

	LayoutScrollBars();
	LayoutContentContainerControl();
	AdjustContentControlSize();
	AdjustScrollBarValues();
}


void ScrollableControl::LayoutScrollBars() {

	const Size& size = GetSize();

	float vertical_scroll_bar_thickness = vertical_scroll_bar_->GetSize().width;
	float horizontal_scroll_bar_thickness = horizontal_scroll_bar_->GetSize().height;

	Rect vertical_scroll_bar_rect(
		size.width - vertical_scroll_bar_thickness,
		0,
		vertical_scroll_bar_thickness,
		size.height - horizontal_scroll_bar_thickness
	);
	vertical_scroll_bar_->SetRect(vertical_scroll_bar_rect);

	Rect horizontal_scroll_bar_rect(
		0,
		size.height - horizontal_scroll_bar_thickness,
		size.width - vertical_scroll_bar_thickness,
		horizontal_scroll_bar_thickness
	);
	horizontal_scroll_bar_->SetRect(horizontal_scroll_bar_rect);
}


void ScrollableControl::LayoutContentContainerControl() {

	Size size = GetSize();
	size.width -= vertical_scroll_bar_->GetWidth();
	size.height -= horizontal_scroll_bar_->GetHeight();

	content_container_control_->SetRect(Rect(Point(), size));
}


void ScrollableControl::AdjustContentControlSize() {

	const Size& content_container_size = content_container_control_->GetSize();
	
	Rect content_rect = content_control_->GetRect();
	content_rect.size = content_size_;

	if (content_rect.size.width < content_container_size.width) {
		content_rect.size.width = content_container_size.width;
		content_rect.position.x = 0;
	}

	if (content_rect.size.height < content_container_size.height) {
		content_rect.size.height = content_container_size.height;
		content_rect.position.y = 0;
	}

	content_control_->SetRect(content_rect);
}


void ScrollableControl::AdjustScrollBarValues() {

	const Size& content_size = content_control_->GetSize();
	const Size& content_container_size = content_container_control_->GetSize();

	vertical_scroll_bar_->SetValueRange(0, content_size.height - content_container_size.height);
	horizontal_scroll_bar_->SetValueRange(0, content_size.width - content_container_size.width);
}


void ScrollableControl::Paint(Canvas& canvas, const Rect& dirty_rect) {
	internal::theme::GetTheme()->GetScrollableControlPainter()->Paint(canvas, dirty_rect, *this);
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

	Rect content_rect = content_control_->GetRect();

	if (scroll_bar == vertical_scroll_bar_) {
		content_rect.position.y = -scroll_bar->GetValue();
	}
	else if (scroll_bar == horizontal_scroll_bar_) {
		content_rect.position.x = -scroll_bar->GetValue();
	}

	content_control_->SetRect(content_rect);
}


}