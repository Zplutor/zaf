#pragma once

namespace zaf::internal {

class WindowStyle {
public:
    static WindowStyle Default() noexcept;
    static WindowStyle FromHandle(HWND handle) noexcept;

public:
    WindowStyle() noexcept = default;
    explicit WindowStyle(DWORD style) noexcept;

    DWORD Value() const noexcept;

    bool IsPopup() const noexcept;
    void SetIsPopup(bool is_popup) noexcept;

    bool HasBorder() const noexcept;
    void SetHasBorder(bool has_border) noexcept;

    bool HasTitleBar() const noexcept;
    void SetHasTitleBar(bool has_title_bar) noexcept;

    bool HasSystemMenu() const noexcept;
    void SetHasSystemMenu(bool has_system_menu) noexcept;

    bool IsSizable() const noexcept;
    void SetIsSizable(bool is_sizable) noexcept;

private:
    DWORD value_{};
};

}