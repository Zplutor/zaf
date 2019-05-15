#include <zaf/window/message/creation.h>
#include <zaf/window/message/hit_test_message.h>
#include <zaf/window/message/keyboard_message.h>
#include <zaf/window/message/message.h>
#include <zaf/window/message/mouse_message.h>

namespace zaf {

std::shared_ptr<Message> CreateMessage(HWND hwnd, UINT id, WPARAM wparam, LPARAM lparam) {

    std::shared_ptr<Message> message;

    switch (id) {
        case WM_NCHITTEST:
            message = std::make_shared<HitTestMessage>();
            break;

        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_NCLBUTTONDOWN:
        case WM_NCLBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_NCRBUTTONDOWN:
        case WM_NCRBUTTONUP:
        case WM_NCMBUTTONDOWN:
        case WM_NCMBUTTONUP:
        case WM_MOUSEMOVE:
        case WM_NCMOUSEMOVE:
        case WM_MOUSELEAVE:
        case WM_NCMOUSELEAVE:
            message = std::make_shared<MouseMessage>();
            break;

        case WM_MOUSEHWHEEL:
        case WM_MOUSEWHEEL:
            message = std::make_shared<MouseWheelMessage>();
            break;

        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
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
        message->wparam = wparam;
        message->lparam = lparam;
    }

    return message;
}


}