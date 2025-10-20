#pragma once

namespace zaf::internal {

class WindowStyle {
public:
    static WindowStyle FromHandle(HWND handle) noexcept;

public:
    WindowStyle() noexcept = default;
    explicit WindowStyle(DWORD style) noexcept;

    DWORD Value() const noexcept;

    bool IsPopup() const noexcept;
    void SetIsPopup(bool is_popup) noexcept;

private:
    DWORD value_{};
};

}