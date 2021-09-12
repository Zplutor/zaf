#include <zaf/internal/message_loop.h>
#include <zaf/window/message/keyboard_message.h>
#include <zaf/window/window.h>

namespace zaf {
namespace internal {

void MessageLoop::Run() {

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {

        if (msg.hwnd) {
            if ((msg.message == WM_KEYDOWN) || (msg.message == WM_SYSKEYDOWN)) {

                auto window = GetWindowFromHandle(msg.hwnd);
                if (window) {

                    Message message{ msg.hwnd, msg.message, msg.wParam, msg.lParam };
                    KeyMessage key_message{ message };
                    bool is_processed = window->PreprocessMessage(key_message);
                    if (is_processed) {
                        continue;
                    }
                }
            }
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

}
}