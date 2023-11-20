#include <zaf/window/window_messager.h>
#include <zaf/base/error/system_error.h>

namespace zaf {

WindowMessager::WindowMessager(HWND window_handle) : window_handle_(window_handle) {

}


LRESULT WindowMessager::Send(UINT message_id, WPARAM wparam, LPARAM lparam) noexcept {
    return SendMessage(window_handle_, message_id, wparam, lparam);
}


void WindowMessager::Post(UINT message_id, WPARAM wparam, LPARAM lparam) {

    BOOL is_succeeded = PostMessage(window_handle_, message_id, wparam, lparam);
    if (!is_succeeded) {
        ZAF_THROW_SYSTEM_ERROR(GetLastError());
    }
}

}