#pragma once

#include <Windows.h>

namespace zaf::internal {

/*
The order of enum values is significant, do not change it.
*/
enum class WindowStyleProperty {

    IsPopup = 0,
    HasBorder,
    HasTitleBar,
    HasSystemMenu,
    IsSizable,
    CanMaximize,
    CanMinimize,

    IsToolWindow,
    IsTopMost,
};


class WindowStyle {
public:
    static WindowStyle Default() noexcept;
    static WindowStyle FromHandle(HWND handle) noexcept;

public:
    WindowStyle() noexcept = default;
    WindowStyle(DWORD basic_value, DWORD extra_value) noexcept;

    bool Has(WindowStyleProperty property) const noexcept;
    void Set(WindowStyleProperty property, bool enable);

    DWORD BasicValue() const noexcept {
        return basic_value_;
    }

    DWORD ExtraValue() const noexcept {
        return extra_value_;
    }

private:
    void SetIsPopup(bool is_popup) noexcept;
    void SetHasBorder(bool has_border);
    void SetHasTitleBar(bool has_title_bar);
    void SetHasSystemMenu(bool has_system_menu);
    void SetIsSizable(bool is_sizable);
    void SetSolo(WindowStyleProperty property, bool enable);

private:
    DWORD basic_value_{};
    DWORD extra_value_{};
};


class WindowExtraStyle {
public:
    WindowExtraStyle() = default;
    explicit WindowExtraStyle(DWORD value);



    DWORD Value() const noexcept {
        return value_;
    }

private:
    DWORD value_{};
};

}