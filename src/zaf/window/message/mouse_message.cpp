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

        if (!IsClientAreaMouseMessage(ID())) {
            return true;
        }

        if (ID() == WM_MOUSEWHEEL || ID() == WM_MOUSEHWHEEL) {
            return true;
        }

        return false;
    }();

    return internal::GetMousePositionFromLPARAM(WindowHandle(), LParam(), need_transform);
}


MouseButton MouseMessage::MouseButton() const {

    switch (ID()) {
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

    if (IsClientAreaMouseMessage(ID())) {
        return static_cast<MouseKey>(WParam());
    }
    
    switch (ID()) {
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

    if (IsClientAreaMouseMessage(ID())) {
        return zaf::HitTestResult::ClientArea;
    }
    else {
        return static_cast<zaf::HitTestResult>(WParam());
    }
}

}