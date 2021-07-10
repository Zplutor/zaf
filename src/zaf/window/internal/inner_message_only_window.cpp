#include <zaf/window/internal/inner_message_only_window.h>
#include <zaf/base/error/system_error.h>
#include <zaf/window/message/creation.h>

namespace zaf::internal {
namespace {

constexpr wchar_t* const MessageOnlyWindowClassName = L"ZafMessageOnlyWindowClass";

}

InnerMessageOnlyWindow::InnerMessageOnlyWindow(HWND parent_window_handle) {

    RegisterWindowClass();

    handle_ = CreateWindowEx(
        0,
        MessageOnlyWindowClassName,
        L"",
        0,
        0,
        0,
        0,
        0,
        parent_window_handle,
        nullptr,
        nullptr,
        nullptr);

    if (!handle_) {
        ZAF_THROW_SYSTEM_ERROR(GetLastError());
    }

    SetLastError(ERROR_SUCCESS);

    SetWindowLongPtr(
        handle_,
        GWLP_USERDATA,
        reinterpret_cast<ULONG_PTR>(this));

    ZAF_THROW_IF_SYSTEM_ERROR(GetLastError());
}


InnerMessageOnlyWindow::~InnerMessageOnlyWindow() {

    DestroyWindow(handle_);
}


void InnerMessageOnlyWindow::RegisterWindowClass() {

    static ATOM atom = []() {

        WNDCLASSEX default_class{};
        default_class.cbSize = sizeof(default_class);
        default_class.lpszClassName = MessageOnlyWindowClassName;
        default_class.lpfnWndProc = WindowProcedure;
        default_class.cbWndExtra = sizeof(LONG_PTR);

        ATOM atom = RegisterClassEx(&default_class);
        if (atom == 0) {
            ZAF_THROW_SYSTEM_ERROR(GetLastError());
        }

        return atom;
    }();
}


LRESULT CALLBACK InnerMessageOnlyWindow::WindowProcedure(
    HWND hwnd,
    UINT id,
    WPARAM wparam,
    LPARAM lparam) {

    LONG_PTR user_data = GetWindowLongPtr(hwnd, GWLP_USERDATA);
    auto window = reinterpret_cast<InnerMessageOnlyWindow*>(user_data);
    if (window) {
        window->OnReceiveMessage(id, wparam, lparam);
    }

    return CallWindowProc(DefWindowProc, hwnd, id, wparam, lparam);
}


void InnerMessageOnlyWindow::OnReceiveMessage(UINT id, WPARAM wparam, LPARAM lparam) {

    auto message = CreateMessage(handle_, id, wparam, lparam);
    subject_.GetObserver().OnNext(*message);
}

}