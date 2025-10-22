#pragma once

#include <Windows.h>

namespace zaf::internal {

enum class WindowStyleProperty : DWORD {
    IsPopup = WS_POPUP,
    HasBorder = WS_BORDER,
    HasTitleBar = WS_CAPTION,
    HasSystemMenu = WS_SYSMENU,
    IsSizable = WS_SIZEBOX,
    CanMaximize = WS_MAXIMIZEBOX,
    CanMinimize = WS_MINIMIZEBOX,
};


class WindowStyle {
public:
    static WindowStyle Default() noexcept;
    static WindowStyle FromHandle(HWND handle) noexcept;

public:
    WindowStyle() noexcept = default;
    explicit WindowStyle(DWORD value) noexcept;

    bool Has(WindowStyleProperty property) const noexcept;
    void Set(WindowStyleProperty property, bool enable);

    DWORD Value() const noexcept;

private:
    void SetIsPopup(bool is_popup) noexcept;
    void SetHasBorder(bool has_border);
    void SetHasTitleBar(bool has_title_bar);
    void SetHasSystemMenu(bool has_system_menu);
    void SetIsSizable(bool is_sizable);
    void SetSolo(WindowStyleProperty property, bool enable);

private:
    DWORD value_{};
};

}