#include <zaf/window/message/mouse_message.h>
#include <windowsx.h>

namespace zaf {

const Point MouseMessage::GetMousePosition() const {

    return Point(
        static_cast<float>(GET_X_LPARAM(lParam)),
        static_cast<float>(GET_Y_LPARAM(lParam)));
}


MouseButton MouseMessage::GetMouseButton() const {

    switch (id) {

        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
            return MouseButton::Left;

        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
            return MouseButton::Middle;

        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
            return MouseButton::Right;

        default:
            return MouseButton::Unspecified;
    }
}


const Point MouseWheelMessage::GetMousePosition() const {

    POINT position;
    position.x = GET_X_LPARAM(lParam);
    position.y = GET_Y_LPARAM(lParam);
    ScreenToClient(hwnd, &position);
    return Point::FromPOINT(position);
}


}