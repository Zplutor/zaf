#include <zaf/control/clickable_control.h>
#include <zaf/base/assert.h>
#include <zaf/base/event_utility.h>
#include <zaf/creation.h>
#include <zaf/graphic/canvas.h>
#include <zaf/object/object_type.h>
#include <zaf/object/type_definition.h>
#include <zaf/rx/subject.h>
#include <zaf/window/message/keyboard_message.h>
#include <zaf/window/message/mouse_message.h>

namespace zaf {

ZAF_DEFINE_TYPE(ClickableControl)
ZAF_DEFINE_TYPE_END


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
    SetCanClick(true);
}


void ClickableControl::Click() {
    RaiseClickEvent();
}


void ClickableControl::OnMouseEnter(const std::shared_ptr<Control>& entered_control) {

    __super::OnMouseEnter(entered_control);

    if (entered_control.get() == this) {
        NeedRepaint();
    }
}


void ClickableControl::OnMouseLeave(const std::shared_ptr<Control>& leaved_control) {

    __super::OnMouseLeave(leaved_control);

    if (leaved_control.get() == this) {
        NeedRepaint();
    }
}


bool ClickableControl::OnMouseMove(const Point& position, const MouseMessage& message) {

    __super::OnMouseMove(position, message);

    CheckIsMousePressed(position, message);
    return true;
}


bool ClickableControl::OnMouseDown(const Point& position, const MouseMessage& message) {

    if (message.MouseButton() == MouseButton::Left) {
        SetIsFocused(true);
        BeginPress(PressType::Mouse);
        CheckIsMousePressed(position, message);
    }
    return true;
}


bool ClickableControl::OnMouseUp(const Point& position, const MouseMessage& message) {

    if (is_mouse_press_) {
        if (message.MouseButton() == MouseButton::Left) {
            EndPress(PressType::Mouse);
            return true;
        }
    }

    return __super::OnMouseUp(position, message);
}


void ClickableControl::OnMouseCapture() {
    is_pressed_ = true;
    NeedRepaint();
}


void ClickableControl::OnMouseRelease() {
    is_pressed_ = false;
    NeedRepaint();
}


bool ClickableControl::OnKeyDown(const KeyMessage& message) {

    if (message.VirtualKey() == VK_SPACE) {
        BeginPress(PressType::Key);
        return true;
    }
    
    return __super::OnKeyDown(message);
}


bool ClickableControl::OnKeyUp(const KeyMessage& message) {

    if (message.VirtualKey() == VK_SPACE) {
        EndPress(PressType::Key);
        return true;
    }

    return __super::OnKeyUp(message);
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

    CaptureMouse();
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
    ReleaseMouse();

    if (is_pressed) {
        Click();
    }
}


void ClickableControl::CheckIsMousePressed(const Point& position, const MouseMessage& message) {

    if (! is_mouse_press_) {
        return;
    }

    bool is_pressed = false;

    if (IsHovered() && IsCapturingMouse()) {
        if (message.PressedMouseKeys() == MouseKey::LeftButton) {

            zaf::Rect bound(Point(), Rect().size);
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


void ClickableControl::OnFocusGain() {
    NeedRepaint();
}


void ClickableControl::OnFocusLose() {
    NeedRepaint();
}

}