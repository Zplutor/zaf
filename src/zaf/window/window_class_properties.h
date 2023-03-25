#pragma once

#include <Windows.h>
#include <string>
#include <string_view>
#include <zaf/base/flag_enum.h>

namespace zaf {

enum class WindowClassStyle {
    None = 0,
    HorizontalRedraw = CS_HREDRAW,
    VerticalRedraw = CS_VREDRAW,
    DropShadow = CS_DROPSHADOW,
};
ZAF_ENABLE_FLAG_ENUM(WindowClassStyle);


class WindowClassProperties {
public:
    WindowClassProperties() {
        inner_.cbSize = sizeof(inner_);
    }

    WindowClassProperties(const WindowClassProperties& other) : 
        name_(other.name_), 
        inner_(other.inner_) {
        inner_.lpszClassName = name_.c_str();
    }

    WindowClassProperties& operator=(const WindowClassProperties& other) {
        name_ = other.name_;
        inner_ = other.inner_;
        inner_.lpszClassName = name_.c_str();
        return *this;
    }

    const std::wstring& Name() const {
        return name_;
    }

    void SetName(std::wstring_view name) {
        name_ = name;
        inner_.lpszClassName = name_.c_str();
    }

    WindowClassStyle Style() const {
        return static_cast<WindowClassStyle>(inner_.style);
    }

    void SetStyle(WindowClassStyle style) {
        inner_.style = static_cast<UINT>(style);
    }

    void SetWindowProcedure(WNDPROC procedure) {
        inner_.lpfnWndProc = procedure;
    }

    void SetBackgroundBrush(HBRUSH brush) {
        inner_.hbrBackground = brush;
    }

    void SetCursor(HCURSOR cursor) {
        inner_.hCursor = cursor;
    }

    void SetIcon(HICON icon) {
        inner_.hIcon = icon;
    }

    void SetSmallIcon(HICON icon) {
        inner_.hIconSm = icon;
    }

    void SetWindowExtraBytes(int bytes) {
        inner_.cbWndExtra = bytes;
    }

    const WNDCLASSEX& Inner() const {
        return inner_;
    }

private:
    std::wstring name_;
    WNDCLASSEX inner_{};
};

}