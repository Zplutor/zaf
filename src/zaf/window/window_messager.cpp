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


void WindowMessager::PostWMSETCURSOR(const MouseMessage& mouse_message) {
    Post(
        WM_SETCURSOR, 
        reinterpret_cast<WPARAM>(mouse_message.WindowHandle()),
        MAKELPARAM(mouse_message.HitTestResult(), mouse_message.ID()));
}


LRESULT WindowMessager::SendWMKEYDOWN(Key key) {
    return SendMessage(window_handle_, WM_KEYDOWN, static_cast<WPARAM>(key), 0);
}

}