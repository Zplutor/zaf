#include <zaf/button.h>
#include <zaf/internal/theme/painter.h>
#include <zaf/internal/theme/theme.h>

namespace zaf {

Button::Button() : 
	is_pressed_(false),
	is_mouse_press_(false),
	is_key_press_(false),
	click_event_(), 
	OnClick(click_event_) {

	SetCanFocused(true);
}


Button::~Button() {

}


void Button::Paint(Canvas& canvas, const Rect& dirty_rect) {
	internal::theme::GetTheme()->GetButtonPainter()->Paint(canvas, dirty_rect, *this);
}


void Button::MouseMove(const Point& position, WPARAM wParam, LPARAM lParam) {

	CheckIsMousePressed(position, wParam);
}


void Button::MouseDown(const Point& position, MouseButton button, WPARAM wParam, LPARAM lParam) {

	if (button == MouseButton::Left) {
		SetIsFocused(true);
		BeginPress(PressType::Mouse);
		CheckIsMousePressed(position, wParam);
	}
}


void Button::MouseUp(const Point& position, MouseButton button, WPARAM wParam, LPARAM lParam) {

	if (button == MouseButton::Left) {
		EndPress(PressType::Mouse);
	}
}


void Button::MouseCapture() {
	is_pressed_ = true;
	NeedRepaint();
}


void Button::MouseRelease() {
	is_pressed_ = false;
	NeedRepaint();
}


void Button::KeyDown(WPARAM wParam, LPARAM lParam) {

	if (wParam == VK_SPACE) {
		BeginPress(PressType::Key);
	}
}


void Button::KeyUp(WPARAM wParam, LPARAM lParam) {

	if (wParam == VK_SPACE) {
		EndPress(PressType::Key);
	}
}


void Button::BeginPress(PressType press_type) {

	switch (press_type) {
	case PressType::Mouse:
		is_mouse_press_ = true;
		break;

	case PressType::Key:
		if (is_mouse_press_) {
			return;
		}
		is_key_press_ = true;
		break;

	default:
		return;
	}

	NeedCaptureMouse(true);
}


void Button::EndPress(PressType press_type) {

	switch (press_type) {
	case PressType::Mouse:
		is_mouse_press_ = false;
		break;

	case PressType::Key:
		is_key_press_ = false;
		break;

	default:
		break;
	}

	if (is_mouse_press_ || is_key_press_) {
		return;
	}

	bool is_pressed = is_pressed_;
	NeedCaptureMouse(false);

	if (is_pressed) {
		click_event_.Trigger(std::dynamic_pointer_cast<Button>(shared_from_this()));
	}
}


void Button::CheckIsMousePressed(const Point& position, WPARAM wParam) {

	if (! is_mouse_press_) {
		return;
	}

	bool is_pressed = false;

	if (IsHovered() && IsCapturingMouse()) {
		if (wParam & MK_LBUTTON) {

			Rect bound(Point(), GetRect().size);
			if (bound.Contain(position)) {
				is_pressed = true;
			}
		}
	}

	bool need_repaint = false;
	if (is_pressed_ != is_pressed) {
		need_repaint = true;
	}

	is_pressed_ = is_pressed;

	if (need_repaint) {
		NeedRepaint();
	}
}

}