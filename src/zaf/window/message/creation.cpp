#include <zaf/window/message/creation.h>
#include <Windowsx.h>
#include <zaf/window/message/message.h>
#include <zaf/window/message/mouse_message.h>

namespace zaf {

static std::shared_ptr<Message> CreateMouseMessage(HWND hwnd, UINT id, WPARAM wParam, LPARAM lParam);
static std::shared_ptr<Message> CreateMouseWheelMessage(HWND hwnd, UINT id, WPARAM wParam, LPARAM lParam);

std::shared_ptr<Message> CreateMessage(HWND hwnd, UINT id, WPARAM wParam, LPARAM lParam) {

    std::shared_ptr<Message> message;

    switch (id) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MOUSEMOVE:
        case WM_MOUSELEAVE:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
            message = CreateMouseMessage(hwnd, id, wParam, lParam);
            break;

        case WM_MOUSEHWHEEL:
        case WM_MOUSEWHEEL:
            message = CreateMouseWheelMessage(hwnd, id, wParam, lParam);
            break;

        default: {
            message = std::make_shared<Message>();
            break;
        }
    }

    if (message != nullptr) {
        message->hwnd = hwnd;
        message->id = id;
        message->wParam = wParam;
        message->lParam = lParam;
    }

    return message;
}


static std::shared_ptr<Message> CreateMouseMessage(HWND hwnd, UINT id, WPARAM wParam, LPARAM lParam) {

    auto message = std::make_shared<MouseMessage>();
    message->position = Point(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)));
    
    switch (id) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
            message->button = MouseButton::Left;
            break;
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
            message->button = MouseButton::Middle;
            break;
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
            message->button = MouseButton::Right;
            break;
        default:
            message->button = MouseButton::Unspecified;
            break;
    }

    return message;
}


static std::shared_ptr<Message> CreateMouseWheelMessage(HWND hwnd, UINT id, WPARAM wParam, LPARAM lParam) {

    auto message = std::make_shared<MouseWheelMessage>();

    POINT position;
    position.x = GET_X_LPARAM(lParam);
    position.y = GET_Y_LPARAM(lParam);
    ScreenToClient(hwnd, &position);
    message->position = Point::FromPOINT(position);

    message->distance = static_cast<short>(HIWORD(wParam));
    message->is_horizontal = (id == WM_MOUSEHWHEEL);
    return message;
}

}