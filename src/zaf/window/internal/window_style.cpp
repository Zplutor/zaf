#include <zaf/window/internal/window_style.h>
#include <zaf/base/error/invalid_operation_error.h>

namespace zaf::internal {

WindowStyle WindowStyle::Default() noexcept {
    return WindowStyle{ 
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_MINIMIZEBOX
    };
}


WindowStyle WindowStyle::FromHandle(HWND handle) noexcept {
    DWORD value = GetWindowLong(handle, GWL_STYLE);
    return WindowStyle{ value };
}


WindowStyle::WindowStyle(DWORD value) noexcept : value_(value) {

}


bool WindowStyle::Has(WindowStyleProperty property) const noexcept {
    auto property_value = static_cast<DWORD>(property);
    return (value_ & property_value) == property_value;
}


void WindowStyle::Set(WindowStyleProperty property, bool enable) {

    switch (property) {
    case WindowStyleProperty::IsPopup:
        SetIsPopup(enable);
        break;
    case WindowStyleProperty::HasBorder:
        SetHasBorder(enable);
        break;
    case WindowStyleProperty::HasTitleBar:
        SetHasTitleBar(enable);
        break;
    case WindowStyleProperty::HasSystemMenu:
        SetHasSystemMenu(enable);
        break;
    case WindowStyleProperty::IsSizable:
        SetIsSizable(enable);
        break;
    default:
        SetSolo(property, enable);
        break;
    }
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


void WindowStyle::SetHasBorder(bool has_border) {

    if (has_border) {
        value_ |= WS_BORDER;
    }
    else {

        if (!Has(WindowStyleProperty::IsPopup)) {
            throw InvalidOperationError(ZAF_SOURCE_LOCATION());
        }

        value_ &= ~WS_CAPTION;
        value_ &= ~WS_SYSMENU;
        value_ &= ~WS_SIZEBOX;
    }
}


void WindowStyle::SetHasTitleBar(bool has_title_bar) {

    if (has_title_bar) {

        if (!Has(WindowStyleProperty::HasBorder)) {
            throw InvalidOperationError(ZAF_SOURCE_LOCATION());
        }

        value_ |= WS_CAPTION;
    }
    else {

        if (!Has(WindowStyleProperty::IsPopup)) {
            throw InvalidOperationError(ZAF_SOURCE_LOCATION());
        }

        bool has_border = Has(WindowStyleProperty::HasBorder);

        value_ &= ~WS_CAPTION;
        value_ &= ~WS_SYSMENU;

        // The border style is kept when title bar is removed.
        if (has_border) {
            value_ |= WS_BORDER;
        }
    }
}


void WindowStyle::SetHasSystemMenu(bool has_system_menu) {

    if (has_system_menu) {

        if (!Has(WindowStyleProperty::HasTitleBar)) {
            throw InvalidOperationError(ZAF_SOURCE_LOCATION());
        }

        value_ |= WS_SYSMENU;
    }
    else {
        value_ &= ~WS_SYSMENU;
    }
}


void WindowStyle::SetIsSizable(bool is_sizable) {

    if (is_sizable) {

        if (!Has(WindowStyleProperty::HasBorder)) {
            throw InvalidOperationError(ZAF_SOURCE_LOCATION());
        }

        value_ |= WS_SIZEBOX;
    }
    else {
        value_ &= ~WS_SIZEBOX;
    }
}


void WindowStyle::SetSolo(WindowStyleProperty property, bool enable) {
    auto property_value = static_cast<DWORD>(property);
    if (enable) {
        value_ |= property_value;
    }
    else {
        value_ &= ~property_value;
    }
}


DWORD WindowStyle::Value() const noexcept {
    return value_;
}

}