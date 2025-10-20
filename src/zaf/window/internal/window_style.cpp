#include <zaf/window/internal/window_style.h>

namespace zaf::internal {

WindowStyle WindowStyle::FromHandle(HWND handle) noexcept {
    DWORD value = GetWindowLong(handle, GWL_STYLE);
    return WindowStyle{ value };
}


WindowStyle::WindowStyle(DWORD style) noexcept : value_(style) {

}


DWORD WindowStyle::Value() const noexcept {
    return value_;
}


bool WindowStyle::IsPopup() const noexcept {
    return (value_ & WS_POPUP) != 0;
}

void WindowStyle::SetIsPopup(bool is_popup) noexcept {
    if (is_popup) {
        value_ |= WS_POPUP;
    }
    else {
        value_ &= ~WS_POPUP;
    }
}

}