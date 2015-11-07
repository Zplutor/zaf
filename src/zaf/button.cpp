#include <zaf/button.h>
#include <zaf/internal/theme/painter.h>
#include <zaf/internal/theme/theme.h>

namespace zaf {

Button::Button() : 
	is_pressed_(false),
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

	CheckIsPressed(position, wParam);
}


void Button::MouseDown(const Point& position, MouseButton button, WPARAM wParam, LPARAM lParam) {

	if (button == MouseButton::Left) {
		NeedCaptureMouse(true);
		SetIsFocused(true);
	}
}


void Button::MouseUp(const Point& position, MouseButton button, WPARAM wParam, LPARAM lParam) {

	if (button == MouseButton::Left) {

		bool is_pressed = IsPressed();

		NeedCaptureMouse(false);

		if (is_pressed) {
			click_event_.Trigger(std::dynamic_pointer_cast<Button>(shared_from_this()));
		}
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


void Button::CheckIsPressed(const Point& position, WPARAM wParam) {

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