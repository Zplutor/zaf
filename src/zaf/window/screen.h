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

    /**
    Transforms a point from screen coordinates to global coordinates.

    @param point_in_screen
        The point in the current screen's coordinates, which is expressed in DIPs.

    @return
        The transformed point in global coordinates, which is expressed in pixels.
    */
    zaf::Point TransformToGlobal(const zaf::Point& point_in_screen) const noexcept;

    /**
    Transforms a rectangle from screen coordinates to global coordinates.

    @param rect_in_screen
        The rectangle in the current screen's coordinates, which is expressed in DIPs.

    @return
        The transformed rectangle in global coordinates, which is expressed in pixels.
    */
    zaf::Rect TransformToGlobal(const zaf::Rect& rect_in_screen) const noexcept;

    /**
    Transforms a point from global coordinates to screen coordinates.

    @param point_in_global
        The point in global coordinates, which is expressed in pixels.

    @return
        The transformed point in the current screen's coordinates, which is expressed in DIPs.
    */
    zaf::Point TransformFromGlobal(const zaf::Point& point_in_global) const noexcept;

    /**
    Transforms a rectangle from global coordinates to screen coordinates.

    @param rect_in_global
        The rectangle in global coordinates, which is expressed in pixels.

    @return
        The transformed rectangle in the current screen's coordinates, which is expressed in DIPs.
    */
    zaf::Rect TransformFromGlobal(const zaf::Rect& rect_in_global) const noexcept;

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