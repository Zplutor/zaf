#pragma once

#include <Windows.h>
#include <string_view>
#include <zaf/graphic/rect.h>

namespace zaf {

class ScreenManager;

class Screen {
public:
    explicit Screen(HMONITOR handle);

    std::wstring_view Name() const noexcept {
        return monitor_info_.szDevice;
    }

    bool IsPrimary() const noexcept {
        return (monitor_info_.dwFlags & MONITORINFOF_PRIMARY) != 0;
    }

    float DPI() const noexcept {
        return dpi_;
    }

    zaf::Rect RectInGlobal() const noexcept;
    zaf::Rect RectInSelf() const noexcept;
    zaf::Size Size() const noexcept;

    zaf::Rect WorkAreaRectInGlobal() const noexcept;
    zaf::Rect WorkAreaRectInSelf() const noexcept;
    zaf::Size WorkAreaSize() const noexcept;

    HMONITOR Handle() const noexcept {
        return handle_;
    }

private:
    friend class ScreenManager;

private:
    void UpdateMonitorInfo();

private:
    HMONITOR handle_{};
    MONITORINFOEX monitor_info_{};
    float dpi_{};
};

}