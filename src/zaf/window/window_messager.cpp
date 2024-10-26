#include "window_messager.h"
#include "window_messager.h"
#include "window_messager.h"
#include <zaf/window/window_messager.h>
#include <zaf/base/error/win32_error.h>
#include <zaf/graphic/dpi.h>

namespace zaf {

WindowMessager::WindowMessager(HWND window_handle) : window_handle_(window_handle) {

}


LRESULT WindowMessager::Send(UINT message_id, WPARAM wparam, LPARAM lparam) noexcept {
    return SendMessage(window_handle_, message_id, wparam, lparam);
}


void WindowMessager::Post(UINT message_id, WPARAM wparam, LPARAM lparam) {

    BOOL is_succeeded = PostMessage(window_handle_, message_id, wparam, lparam);
    if (!is_succeeded) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }
}


void WindowMessager::PostWMSETCURSOR(const MouseMessage& mouse_message) {
    Post(
        WM_SETCURSOR, 
        reinterpret_cast<WPARAM>(mouse_message.WindowHandle()),
        MAKELPARAM(mouse_message.HitTestResult(), mouse_message.ID()));
}


LRESULT WindowMessager::SendWMLBUTTONDOWN(const Point& position) {
    return SendMessage(window_handle_, WM_LBUTTONDOWN, MK_LBUTTON, ToLPARAM(position));
}


LRESULT WindowMessager::SendWMLBUTTONUP(const Point& position) {
    return SendMessage(window_handle_, WM_LBUTTONUP, 0, ToLPARAM(position));
}


LRESULT WindowMessager::SendWMRBUTTONDOWN(const Point& position) {
    return SendMessage(window_handle_, WM_RBUTTONDOWN, MK_RBUTTON, ToLPARAM(position));
}


LRESULT WindowMessager::SendWMRBUTTONUP(const Point& position) {
    return SendMessage(window_handle_, WM_RBUTTONUP, 0, ToLPARAM(position));
}


LRESULT WindowMessager::SendWMMOUSEMOVE(const Point& position) {
    return SendMessage(window_handle_, WM_MOUSEMOVE, 0, ToLPARAM(position));
}


LRESULT WindowMessager::SendWMKEYDOWN(Key key) {
    return SendMessage(window_handle_, WM_KEYDOWN, static_cast<WPARAM>(key), 0);
}


LPARAM WindowMessager::ToLPARAM(const Point& point) const {

    auto dpi = static_cast<float>(GetDpiForWindow(window_handle_));
    auto point_in_pixels = FromDIPs(point, dpi);
    return MAKELPARAM(static_cast<WORD>(point_in_pixels.x), static_cast<WORD>(point_in_pixels.y));
}

}