#include <zaf/window/internal/window_style.h>

namespace zaf::internal {

WindowStyle WindowStyle::Default() noexcept {
    return WindowStyle{ WS_OVERLAPPED | WS_CAPTION };
}


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
        value_ |= WS_CAPTION;
    }
}


bool WindowStyle::HasBorder() const noexcept {
    return (value_ & WS_BORDER) != 0;
}

void WindowStyle::SetHasBorder(bool has_border) noexcept {
    if (has_border) {
        value_ |= WS_BORDER;
    }
    else {
        value_ &= ~WS_CAPTION;
        value_ &= ~WS_SYSMENU;
        value_ &= ~WS_SIZEBOX;
    }
}


bool WindowStyle::HasTitleBar() const noexcept {
    return (value_ & WS_CAPTION) != 0;
}

void WindowStyle::SetHasTitleBar(bool has_title_bar) noexcept {
    if (has_title_bar) {
        value_ |= WS_CAPTION;
    }
    else {

        bool has_border = HasBorder();

        value_ &= ~WS_CAPTION;
        value_ &= ~WS_SYSMENU;

        // The border style is kept when title bar is removed.
        if (has_border) {
            value_ |= WS_BORDER;
        }
    }
}


bool WindowStyle::HasSystemMenu() const noexcept {
    return (value_ & WS_SYSMENU) != 0;
}

void WindowStyle::SetHasSystemMenu(bool has_system_menu) noexcept {
    if (has_system_menu) {
        value_ |= WS_SYSMENU;
    }
    else {
        value_ &= ~WS_SYSMENU;
    }
}


bool WindowStyle::IsSizable() const noexcept {
    return (value_ & WS_SIZEBOX) != 0;
}

void WindowStyle::SetIsSizable(bool is_sizable) noexcept {
    if (is_sizable) {
        value_ |= WS_SIZEBOX;
    }
    else {
        value_ &= ~WS_SIZEBOX;
    }
}

}