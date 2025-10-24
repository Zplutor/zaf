#pragma once

#include <Windows.h>

namespace zaf::internal {

enum class WindowBasicStyleProperty : DWORD {
    IsPopup = WS_POPUP,
    HasBorder = WS_BORDER,
    HasTitleBar = WS_CAPTION,
    HasSystemMenu = WS_SYSMENU,
    IsSizable = WS_SIZEBOX,
    CanMaximize = WS_MAXIMIZEBOX,
    CanMinimize = WS_MINIMIZEBOX,
};


class WindowBasicStyle {
public:
    static WindowBasicStyle Default() noexcept;
    static WindowBasicStyle FromHandle(HWND handle) noexcept;

public:
    WindowBasicStyle() noexcept = default;
    explicit WindowBasicStyle(DWORD value) noexcept;

    bool Has(WindowBasicStyleProperty property) const noexcept;
    void Set(WindowBasicStyleProperty property, bool enable);

    DWORD Value() const noexcept {
        return value_;
    }

private:
    void SetIsPopup(bool is_popup) noexcept;
    void SetHasBorder(bool has_border);
    void SetHasTitleBar(bool has_title_bar);
    void SetHasSystemMenu(bool has_system_menu);
    void SetIsSizable(bool is_sizable);
    void SetSolo(WindowBasicStyleProperty property, bool enable);

private:
    DWORD value_{};
};


enum class WindowExtendedStyleProperty : DWORD {
    IsToolWindow = WS_EX_TOOLWINDOW,
    IsTopMost = WS_EX_TOPMOST,
};

class WindowExtendedStyle {
public:
    static WindowExtendedStyle FromHandle(HWND handle) noexcept;

public:
    WindowExtendedStyle() = default;
    explicit WindowExtendedStyle(DWORD value) noexcept;

    bool Has(WindowExtendedStyleProperty property) const noexcept;
    void Set(WindowExtendedStyleProperty property, bool enable) noexcept;

    DWORD Value() const noexcept {
        return value_;
    }

private:
    DWORD value_{};
};

}