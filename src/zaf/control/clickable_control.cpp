#include <zaf/control/clickable_control.h>
#include <zaf/creation.h>
#include <zaf/graphic/canvas.h>
#include <zaf/object/object_type.h>
#include <zaf/rx/subject.h>
#include <zaf/window/message/keyboard_message.h>
#include <zaf/window/message/mouse_message.h>

namespace zaf {

ZAF_OBJECT_IMPL(ClickableControl);


ClickableControl::ClickableControl() :
    is_pressed_(false),
    is_mouse_press_(false),
    is_key_press_(false) {

}


ClickableControl::~ClickableControl() {

}


void ClickableControl::Initialize() {
    __super::Initialize();
    SetCanFocus(true);
}


void ClickableControl::Click() {

    ClickInfo click_info{ As<ClickableControl>(shared_from_this()) };
    OnClick(click_info);
}


void ClickableControl::OnClick(const ClickInfo& event_info) {
    click_event_.Raise(event_info);
}


rx::Observable<ClickInfo> ClickableControl::ClickEvent() const {
    return click_event_.GetObservable();
}


void ClickableControl::OnMouseMove(const MouseMoveInfo& event_info) {

    __super::OnMouseMove(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    bool is_handled = CheckIsMousePressed(event_info.PositionAtSender(), event_info.Message());
    if (is_handled) {
        event_info.MarkAsHandled();
    }
}


void ClickableControl::OnMouseDown(const MouseDownInfo& event_info) {

    __super::OnMouseDown(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    if (event_info.Message().MouseButton() == MouseButton::Left) {

        SetIsFocused(true);
        BeginPress(PressType::Mouse);
        CheckIsMousePressed(event_info.PositionAtSender(), event_info.Message());

        event_info.MarkAsHandled();
    }
}


void ClickableControl::OnMouseUp(const MouseUpInfo& event_info) {

    __super::OnMouseUp(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    if (is_mouse_press_) {
        if (event_info.Message().MouseButton() == MouseButton::Left) {
            EndPress(PressType::Mouse);
            event_info.MarkAsHandled();
        }
    }
}


void ClickableControl::OnMouseCaptured(const MouseCapturedInfo& event_info) {

    __super::OnMouseCaptured(event_info);

    is_pressed_ = true;
    NeedUpdateStyle();
    NeedRepaint();
}


void ClickableControl::OnMouseReleased(const MouseReleasedInfo& event_info) {

    __super::OnMouseReleased(event_info);

    is_pressed_ = false;
    NeedUpdateStyle();
    NeedRepaint();
}


void ClickableControl::OnKeyDown(const KeyDownInfo& event_info) {

    __super::OnKeyDown(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    if (event_info.Message().Key() == Key::Space) {
        BeginPress(PressType::Key);
        event_info.MarkAsHandled();
    }
}


void ClickableControl::OnKeyUp(const KeyUpInfo& event_info) {

    __super::OnKeyUp(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    if (event_info.Message().Key() == Key::Space) {
        EndPress(PressType::Key);
        event_info.MarkAsHandled();
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


bool ClickableControl::CheckIsMousePressed(const Point& position, const MouseMessage& message) {

    if (! is_mouse_press_) {
        return false;
    }

    bool is_pressed = false;

    if (IsMouseOver() && IsCapturingMouse()) {
        if (message.PressedMouseKeys() == MouseKey::LeftButton) {

            zaf::Rect bound(Point(), Rect().size);
            if (bound.Contains(position)) {
                is_pressed = true;
            }
        }
    }

    bool need_repaint = false;
    if (is_pressed_ != is_pressed) {
        need_repaint = true;
    }

    is_pressed_ = is_pressed;
    NeedUpdateStyle();

    if (need_repaint) {
        NeedRepaint();
    }

    return true;
}

}