#include <zaf/window/screen.h>
#include <ShellScalingApi.h>
#include <zaf/base/error/com_error.h>
#include <zaf/base/error/precondition_error.h>
#include <zaf/base/error/win32_error.h>
#include <zaf/graphic/dpi.h>

namespace zaf {

Screen::Screen(HMONITOR handle) : handle_(handle) {
    ZAF_EXPECT(handle_);
    UpdateMonitorInfo();
}


void Screen::UpdateMonitorInfo() {

    MONITORINFOEX monitor_info{};
    monitor_info.cbSize = sizeof(monitor_info);
    BOOL is_succeeded = GetMonitorInfo(handle_, &monitor_info);
    if (!is_succeeded) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }

    UINT dpi_x{};
    UINT dpi_y{};
    HRESULT hresult = GetDpiForMonitor(handle_, MDT_EFFECTIVE_DPI, &dpi_x, &dpi_y);
    if (FAILED(hresult)) {
        ZAF_THROW_COM_ERROR(hresult);
    }

    monitor_info_ = monitor_info;
    dpi_ = static_cast<float>(dpi_x);
}


zaf::Rect Screen::RectInGlobal() const noexcept {
    return zaf::Rect::FromRECT(monitor_info_.rcMonitor);
}


zaf::Rect Screen::RectInSelf() const noexcept {
    return zaf::Rect{ zaf::Point{}, this->Size() };
}


zaf::Size Screen::Size() const noexcept {
    zaf::Size result{
        static_cast<float>(monitor_info_.rcMonitor.right - monitor_info_.rcMonitor.left),
        static_cast<float>(monitor_info_.rcMonitor.bottom - monitor_info_.rcMonitor.top)
    };
    return ToDIPs(result, dpi_);
}


zaf::Rect Screen::WorkAreaRectInGlobal() const noexcept {
    return zaf::Rect::FromRECT(monitor_info_.rcWork);
}


zaf::Rect Screen::WorkAreaRectInSelf() const noexcept {
    zaf::Point position{
        static_cast<float>(monitor_info_.rcWork.left - monitor_info_.rcMonitor.left),
        static_cast<float>(monitor_info_.rcWork.top - monitor_info_.rcMonitor.top)
    };
    position = ToDIPs(position, dpi_);
    return zaf::Rect{ position, this->WorkAreaSize() };
}


zaf::Size Screen::WorkAreaSize() const noexcept {
    zaf::Size result{
        static_cast<float>(monitor_info_.rcWork.right - monitor_info_.rcWork.left),
        static_cast<float>(monitor_info_.rcWork.bottom - monitor_info_.rcWork.top)
    };
    return ToDIPs(result, dpi_);
}


zaf::Point Screen::TransformToGlobal(const zaf::Point& point_in_screen) const noexcept {
    auto result = FromDIPs(point_in_screen, dpi_);
    result.AddOffset(this->RectInGlobal().position);
    return result;
}


zaf::Rect Screen::TransformToGlobal(const zaf::Rect& rect_in_screen) const noexcept {
    auto result = FromDIPs(rect_in_screen, dpi_);
    result.AddOffset(this->RectInGlobal().position);
    return result;
}


zaf::Point Screen::TransformFromGlobal(const zaf::Point& point_in_global) const noexcept {
    auto result = point_in_global;
    result.SubtractOffset(this->RectInGlobal().position);
    return ToDIPs(result, dpi_);
}


zaf::Rect Screen::TransformFromGlobal(const zaf::Rect& rect_in_global) const noexcept {
    auto result = rect_in_global;
    result.SubtractOffset(this->RectInGlobal().position);
    return ToDIPs(result, dpi_);
}

}