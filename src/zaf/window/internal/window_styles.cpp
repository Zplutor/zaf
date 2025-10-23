#include <zaf/window/internal/window_styles.h>
#include <zaf/base/error/invalid_operation_error.h>

namespace zaf::internal {
namespace {

bool HasFlag(DWORD value, DWORD flag) noexcept {
    return (value & flag) == flag;
}

void SetFlag(DWORD& value, DWORD flag, bool enable) noexcept {
    if (enable) {
        value |= flag;
    }
    else {
        value &= ~flag;
    }
}

} // namespace

WindowBasicStyle WindowBasicStyle::Default() noexcept {
    return WindowBasicStyle{
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
    };
}


WindowBasicStyle WindowBasicStyle::FromHandle(HWND handle) noexcept {
    DWORD value = GetWindowLong(handle, GWL_STYLE);
    return WindowBasicStyle{ value };
}


WindowBasicStyle::WindowBasicStyle(DWORD value) noexcept : value_(value) {

}


bool WindowBasicStyle::Has(WindowBasicStyleProperty property) const noexcept {
    return HasFlag(value_, static_cast<DWORD>(property));
}


void WindowBasicStyle::Set(WindowBasicStyleProperty property, bool enable) {

    switch (property) {
    case WindowBasicStyleProperty::IsPopup:
        SetIsPopup(enable);
        break;
    case WindowBasicStyleProperty::HasBorder:
        SetHasBorder(enable);
        break;
    case WindowBasicStyleProperty::HasTitleBar:
        SetHasTitleBar(enable);
        break;
    case WindowBasicStyleProperty::HasSystemMenu:
        SetHasSystemMenu(enable);
        break;
    case WindowBasicStyleProperty::IsSizable:
        SetIsSizable(enable);
        break;
    default:
        SetSolo(property, enable);
        break;
    }
}


void WindowBasicStyle::SetIsPopup(bool is_popup) noexcept {
    if (is_popup) {
        value_ |= WS_POPUP;
    }
    else {
        value_ &= ~WS_POPUP;
        value_ |= WS_CAPTION;
    }
}


void WindowBasicStyle::SetHasBorder(bool has_border) {

    if (has_border) {
        value_ |= WS_BORDER;
    }
    else {

        if (!Has(WindowBasicStyleProperty::IsPopup)) {
            throw InvalidOperationError(ZAF_SOURCE_LOCATION());
        }

        value_ &= ~WS_CAPTION;
        value_ &= ~WS_SYSMENU;
        value_ &= ~WS_SIZEBOX;
    }
}


void WindowBasicStyle::SetHasTitleBar(bool has_title_bar) {

    if (has_title_bar) {

        if (!Has(WindowBasicStyleProperty::HasBorder)) {
            throw InvalidOperationError(ZAF_SOURCE_LOCATION());
        }

        value_ |= WS_CAPTION;
    }
    else {

        if (!Has(WindowBasicStyleProperty::IsPopup)) {
            throw InvalidOperationError(ZAF_SOURCE_LOCATION());
        }

        bool has_border = Has(WindowBasicStyleProperty::HasBorder);

        value_ &= ~WS_CAPTION;
        value_ &= ~WS_SYSMENU;

        // The border style is kept when title bar is removed.
        if (has_border) {
            value_ |= WS_BORDER;
        }
    }
}


void WindowBasicStyle::SetHasSystemMenu(bool has_system_menu) {

    if (has_system_menu) {

        if (!Has(WindowBasicStyleProperty::HasTitleBar)) {
            throw InvalidOperationError(ZAF_SOURCE_LOCATION());
        }

        value_ |= WS_SYSMENU;
    }
    else {
        value_ &= ~WS_SYSMENU;
    }
}


void WindowBasicStyle::SetIsSizable(bool is_sizable) {

    if (is_sizable) {

        if (!Has(WindowBasicStyleProperty::HasBorder)) {
            throw InvalidOperationError(ZAF_SOURCE_LOCATION());
        }

        value_ |= WS_SIZEBOX;
    }
    else {
        value_ &= ~WS_SIZEBOX;
    }
}


void WindowBasicStyle::SetSolo(WindowBasicStyleProperty property, bool enable) {
    SetFlag(value_, static_cast<DWORD>(property), enable);
}


WindowExtraStyle WindowExtraStyle::FromHandle(HWND handle) noexcept {
    DWORD value = GetWindowLong(handle, GWL_EXSTYLE);
    return WindowExtraStyle{ value };
}


WindowExtraStyle::WindowExtraStyle(DWORD value) noexcept : value_(value) {

}


bool WindowExtraStyle::Has(WindowExtraStyleProperty property) const noexcept {
    return HasFlag(value_, static_cast<DWORD>(property));
}


void WindowExtraStyle::Set(WindowExtraStyleProperty property, bool enable) noexcept {
    SetFlag(value_, static_cast<DWORD>(property), enable);
}

}