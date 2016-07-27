#include <zaf/window/message/creation.h>
#include <zaf/window/message/keyboard_message.h>
#include <zaf/window/message/message.h>
#include <zaf/window/message/mouse_message.h>

namespace zaf {

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
            message = std::make_shared<MouseMessage>();
            break;

        case WM_MOUSEHWHEEL:
        case WM_MOUSEWHEEL:
            message = std::make_shared<MouseWheelMessage>();
            break;

        case WM_KEYDOWN:
        case WM_KEYUP:
            message = std::make_shared<KeyMessage>();
            break;

        case WM_CHAR:
            message = std::make_shared<CharMessage>();
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


}