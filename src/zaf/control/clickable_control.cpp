#include <zaf/control/clickable_control.h>
#include <zaf/base/event_utility.h>
#include <zaf/creation.h>
#include <zaf/graphic/canvas.h>
#include <zaf/object/object_type.h>
#include <zaf/object/type_definition.h>
#include <zaf/rx/subject.h>
#include <zaf/window/message/keyboard_message.h>
#include <zaf/window/message/mouse_message.h>

namespace zaf {
namespace {

constexpr const wchar_t* const ClickEventPropertyName = L"ClickEvent";

}

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
}


void ClickableControl::Click() {
    OnClick();
}


void ClickableControl::OnClick() {

    auto observer = GetEventObserver<ClickableControlClickInfo>(
        GetPropertyMap(),
        ClickEventPropertyName);

    if (observer) {
        observer->OnNext(ClickableControlClickInfo{ As<ClickableControl>(shared_from_this()) });
    }
}


void ClickableControl::OnMouseEnter(const MouseEnterInfo& event_info) {

    __super::OnMouseEnter(event_info);

    if (event_info.Source().get() == this) {
        NeedRepaint();
    }
}


void ClickableControl::OnMouseLeave(const MouseLeaveInfo& event_info) {

    __super::OnMouseLeave(event_info);

    if (event_info.Source().get() == this) {
        NeedRepaint();
    }
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


void ClickableControl::OnMouseCapture() {
    is_pressed_ = true;
    NeedRepaint();
}


void ClickableControl::OnMouseRelease() {
    is_pressed_ = false;
    NeedRepaint();
}


void ClickableControl::OnKeyDown(const KeyDownInfo& event_info) {

    __super::OnKeyDown(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    if (event_info.Message().VirtualKey() == VK_SPACE) {
        BeginPress(PressType::Key);
        event_info.MarkAsHandled();
    }
}


void ClickableControl::OnKeyUp(const KeyUpInfo& event_info) {

    __super::OnKeyUp(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    if (event_info.Message().VirtualKey() == VK_SPACE) {
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

    return true;
}


void ClickableControl::OnFocusChanged() {

    NeedRepaint();

    __super::OnFocusChanged();
}


Observable<ClickableControlClickInfo> ClickableControl::ClickEvent() {
    return GetEventObservable<ClickableControlClickInfo>(GetPropertyMap(), ClickEventPropertyName);
}

}