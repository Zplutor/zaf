#include <zaf/control/clickable_control.h>
#include <zaf/base/assert.h>
#include <zaf/base/event_utility.h>
#include <zaf/graphic/canvas.h>
#include <zaf/window/message/key_message.h>
#include <zaf/window/message/mouse_message.h>

namespace zaf {

static const wchar_t* const kClickEventPropertyName = L"ClickEvent";

ClickableControl::ClickableControl() :
	is_pressed_(false),
	is_mouse_press_(false),
	is_key_press_(false) {

}


ClickableControl::~ClickableControl() {

}


void ClickableControl::Initialize() {
	__super::Initialize();
	SetCanFocused(true);
}


ClickableControl::ClickEvent::Proxy ClickableControl::GetClickEvent() {
    return GetEventProxyFromPropertyMap<ClickEvent>(GetPropertyMap(), kClickEventPropertyName);
}


void ClickableControl::Click() {

    MouseClick();

    auto click_event = TryGetEventFromPropertyMap<ClickEvent>(GetPropertyMap(), kClickEventPropertyName);
    if (click_event != nullptr) {
        click_event->Trigger(std::dynamic_pointer_cast<ClickableControl>(shared_from_this()));
    }
}


void ClickableControl::MouseEnter() {
	NeedRepaint();
}


void ClickableControl::MouseLeave() {
	NeedRepaint();
}


void ClickableControl::MouseMove(const Point& position, const MouseMessage& message) {

	CheckIsMousePressed(position, message.wParam);
}


void ClickableControl::MouseDown(const Point& position, const MouseMessage& message) {

	if (message.GetMouseButton() == MouseButton::Left) {
		SetIsFocused(true);
		BeginPress(PressType::Mouse);
		CheckIsMousePressed(position, message.wParam);
	}
}


void ClickableControl::MouseUp(const Point& position, const MouseMessage& message) {

    if (message.GetMouseButton() == MouseButton::Left) {
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


bool ClickableControl::KeyDown(const KeyMessage& message) {

	if (message.wParam == VK_SPACE) {
		BeginPress(PressType::Key);
        return true;
	}
    
    return __super::KeyDown(message);
}


bool ClickableControl::KeyUp(const KeyMessage& message) {

	if (message.wParam == VK_SPACE) {
		EndPress(PressType::Key);
        return true;
	}

    return __super::KeyUp(message);
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
        Click();
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