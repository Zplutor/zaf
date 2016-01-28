#include <zaf/control/clickable_control.h>
#include <zaf/base/assert.h>
#include <zaf/graphic/canvas.h>

namespace zaf {

static const wchar_t* const kPressedBackgroundColorPropertyName = L"PressedBackgroundColor";
static const wchar_t* const kPressedBorderColorPropertyName = L"PressedBorderColor";
static const wchar_t* const kPressedForegroundColorPropertyName = L"PressedForegroundColor";

ClickableControl::ClickableControl() :
	is_pressed_(false),
	is_mouse_press_(false),
	is_key_press_(false),
	click_event_(),
	OnClick(click_event_) {

	SetCanFocused(true);
}


ClickableControl::~ClickableControl() {

}


const Color ClickableControl::GetColor(int paint_component, int paint_state) const {

	if (paint_state == PaintState::Pressed) {

		auto get_pressed_color = [paint_component, this](const std::wstring& property_name) {

			auto color = GetPropertyMap().TryGetProperty<Color>(property_name);
			if (color != nullptr) {
				return *color;
			}

			return Control::GetColor(paint_component, Control::PaintState::Hovered);
		};

		switch (paint_component) {

			case PaintComponent::Background: 
				return get_pressed_color(kPressedBackgroundColorPropertyName);
				
			case PaintComponent::Foreground: 
				return get_pressed_color(kPressedForegroundColorPropertyName);

			case PaintComponent::Border:
				return get_pressed_color(kPressedBorderColorPropertyName);

			default:
				break;
		}
	}
	
	return Control::GetColor(paint_component, paint_state);
}


void ClickableControl::SetColor(int paint_component, int paint_state, const Color& color) {

	if (paint_state == PaintState::Pressed) {

		std::wstring property_name;

		switch (paint_component) {

			case PaintComponent::Background:
				property_name = kPressedBackgroundColorPropertyName;
				break;

			case PaintComponent::Foreground:
				property_name = kPressedForegroundColorPropertyName;
				break;

			case PaintComponent::Border:
				property_name= kPressedBorderColorPropertyName;
				break;

			default:
				break;
		}

		if (! property_name.empty()) {
			GetPropertyMap().SetProperty(property_name, color);
			NeedRepaint();
			return;
		}
	}

	Control::SetColor(paint_component, paint_state, color);
}


int ClickableControl::GetPaintState() const {

	if (IsHovered() && IsPressed()) {
		return PaintState::Pressed;
	}

	if (IsHovered() ^ IsPressed()) {
		return Control::PaintState::Hovered;
	}

	return Control::GetPaintState();
}


void ClickableControl::MouseEnter() {
	NeedRepaint();
}


void ClickableControl::MouseLeave() {
	NeedRepaint();
}


void ClickableControl::MouseMove(const Point& position, WPARAM wParam, LPARAM lParam) {

	CheckIsMousePressed(position, wParam);
}


void ClickableControl::MouseDown(const Point& position, MouseButton button, WPARAM wParam, LPARAM lParam) {

	if (button == MouseButton::Left) {
		SetIsFocused(true);
		BeginPress(PressType::Mouse);
		CheckIsMousePressed(position, wParam);
	}
}


void ClickableControl::MouseUp(const Point& position, MouseButton button, WPARAM wParam, LPARAM lParam) {

	if (button == MouseButton::Left) {
		EndPress(PressType::Mouse);
	}
}


void ClickableControl::MouseCapture() {
	is_pressed_ = true;
	NeedRepaint();
}


void ClickableControl::MouseRelease() {
	is_pressed_ = false;
	NeedRepaint();
}


void ClickableControl::KeyDown(WPARAM wParam, LPARAM lParam) {

	if (wParam == VK_SPACE) {
		BeginPress(PressType::Key);
	}
}


void ClickableControl::KeyUp(WPARAM wParam, LPARAM lParam) {

	if (wParam == VK_SPACE) {
		EndPress(PressType::Key);
	}
}


void ClickableControl::BeginPress(PressType press_type) {

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


void ClickableControl::EndPress(PressType press_type) {

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
		MouseClick();
		click_event_.Trigger(std::dynamic_pointer_cast<ClickableControl>(shared_from_this()));
	}
}


void ClickableControl::CheckIsMousePressed(const Point& position, WPARAM wParam) {

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


void ClickableControl::MouseClick() {

}


void ClickableControl::FocusGain() {
	NeedRepaint();
}


void ClickableControl::FocusLose() {
	NeedRepaint();
}

}