#include <zaf/window/internal/window_style.h>
#include <zaf/base/error/invalid_operation_error.h>

namespace zaf::internal {
namespace {

constexpr DWORD BasicStyleValues[] = {
    WS_POPUP,
    WS_BORDER,
    WS_CAPTION,
    WS_SYSMENU,
    WS_SIZEBOX,
    WS_MAXIMIZEBOX,
    WS_MINIMIZEBOX,
};

constexpr std::size_t BasicStyleCount = std::size(BasicStyleValues);

constexpr DWORD ExtraStyleValues[] = {
    WS_EX_TOOLWINDOW,
    WS_EX_TOPMOST,
};

struct StyleValue {
    DWORD value{};
    bool is_extra{};
};

StyleValue ToStyleValue(WindowStyleProperty property) {
    auto index = static_cast<std::size_t>(property);
    if (index < BasicStyleCount) {
        return { BasicStyleValues[index], false };
    }
    index -= BasicStyleCount;
    return { ExtraStyleValues[index], true };
}

} // namespace

WindowStyle WindowStyle::Default() noexcept {
    return WindowStyle{ 
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
        0
    };
}


WindowStyle WindowStyle::FromHandle(HWND handle) noexcept {
    DWORD basic_value = GetWindowLong(handle, GWL_STYLE);
    DWORD extra_value = GetWindowLong(handle, GWL_EXSTYLE);
    return WindowStyle{ basic_value, extra_value };
}


WindowStyle::WindowStyle(DWORD basic_value, DWORD extra_value) noexcept :
    basic_value_(basic_value),
    extra_value_(extra_value) {

}


bool WindowStyle::Has(WindowStyleProperty property) const noexcept {
    auto style_value = ToStyleValue(property);
    auto current_value = style_value.is_extra ? extra_value_ : basic_value_;
    return (current_value & style_value.value) == style_value.value;
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
        basic_value_ |= WS_POPUP;
    }
    else {
        basic_value_ &= ~WS_POPUP;
        basic_value_ |= WS_CAPTION;
    }
}


void WindowStyle::SetHasBorder(bool has_border) {

    if (has_border) {
        basic_value_ |= WS_BORDER;
    }
    else {

        if (!Has(WindowStyleProperty::IsPopup)) {
            throw InvalidOperationError(ZAF_SOURCE_LOCATION());
        }

        basic_value_ &= ~WS_CAPTION;
        basic_value_ &= ~WS_SYSMENU;
        basic_value_ &= ~WS_SIZEBOX;
    }
}


void WindowStyle::SetHasTitleBar(bool has_title_bar) {

    if (has_title_bar) {

        if (!Has(WindowStyleProperty::HasBorder)) {
            throw InvalidOperationError(ZAF_SOURCE_LOCATION());
        }

        basic_value_ |= WS_CAPTION;
    }
    else {

        if (!Has(WindowStyleProperty::IsPopup)) {
            throw InvalidOperationError(ZAF_SOURCE_LOCATION());
        }

        bool has_border = Has(WindowStyleProperty::HasBorder);

        basic_value_ &= ~WS_CAPTION;
        basic_value_ &= ~WS_SYSMENU;

        // The border style is kept when title bar is removed.
        if (has_border) {
            basic_value_ |= WS_BORDER;
        }
    }
}


void WindowStyle::SetHasSystemMenu(bool has_system_menu) {

    if (has_system_menu) {

        if (!Has(WindowStyleProperty::HasTitleBar)) {
            throw InvalidOperationError(ZAF_SOURCE_LOCATION());
        }

        basic_value_ |= WS_SYSMENU;
    }
    else {
        basic_value_ &= ~WS_SYSMENU;
    }
}


void WindowStyle::SetIsSizable(bool is_sizable) {

    if (is_sizable) {

        if (!Has(WindowStyleProperty::HasBorder)) {
            throw InvalidOperationError(ZAF_SOURCE_LOCATION());
        }

        basic_value_ |= WS_SIZEBOX;
    }
    else {
        basic_value_ &= ~WS_SIZEBOX;
    }
}


void WindowStyle::SetSolo(WindowStyleProperty property, bool enable) {

    auto style_value = ToStyleValue(property);
    auto& current_value = style_value.is_extra ? extra_value_ : basic_value_;
    if (enable) {
        current_value |= style_value.value;
    }
    else {
        current_value &= ~style_value.value;
    }
}

}