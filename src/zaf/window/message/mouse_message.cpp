#include <zaf/window/message/mouse_message.h>
#include <windowsx.h>
#include <zaf/window/message/internal/mouse_position_conversion.h>

namespace zaf {
namespace {

bool IsClientAreaMouseMessage(UINT message_id) {
    return (WM_MOUSEFIRST <= message_id) && (message_id <= WM_MOUSELAST);
}

}

Point MouseMessage::MousePosition() const {

    bool need_transform = [this]() {

        if (!IsClientAreaMouseMessage(Inner().id)) {
            return true;
        }

        if (Inner().id == WM_MOUSEWHEEL || Inner().id == WM_MOUSEHWHEEL) {
            return true;
        }

        return false;
    }();

    return internal::GetMousePositionFromLPARAM(
        Inner().lparam, need_transform ? Inner().hwnd : nullptr);
}


MouseButton MouseMessage::MouseButton() const {

    switch (Inner().id) {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_NCLBUTTONDOWN:
    case WM_NCLBUTTONUP:
        return MouseButton::Left;

    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_NCMBUTTONDOWN:
    case WM_NCMBUTTONUP:
        return MouseButton::Middle;

    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_NCRBUTTONDOWN:
    case WM_NCRBUTTONUP:
        return MouseButton::Right;
            
    default:
        return MouseButton::Unspecified;
    }
}


MouseKey MouseMessage::PressedMouseKeys() const {

    if (IsClientAreaMouseMessage(Inner().id)) {
        return static_cast<MouseKey>(Inner().wparam);
    }
    
    switch (Inner().id) {
    case WM_NCLBUTTONDOWN:
        return MouseKey::LeftButton;
    case WM_NCMBUTTONDOWN:
        return MouseKey::MiddleButton;
    case WM_NCRBUTTONDOWN:
        return MouseKey::RightButton;
    default:
        return MouseKey::None;
    }
}


zaf::HitTestResult MouseMessage::HitTestResult() const {

    if (IsClientAreaMouseMessage(Inner().id)) {
        return zaf::HitTestResult::ClientArea;
    }
    else {
        return static_cast<zaf::HitTestResult>(Inner().wparam);
    }
}

}