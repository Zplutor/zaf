#pragma once

#include <zaf/base/error/win32_error.h>
#include <zaf/window/internal/window_handle_state_data.h>
#include <zaf/window/internal/window_not_created_state_data.h>
#include <zaf/window/internal/window_styles.h>

namespace zaf::internal {

template<typename PROPERTY>
class WindowStyleShim;

template<>
class WindowStyleShim<WindowBasicStyleProperty> {
public:
    static bool Has(
        const WindowNotCreatedStateData& state_data, 
        WindowBasicStyleProperty property) noexcept {

        return state_data.basic_style.Has(property);
    }

    static bool Has(
        const WindowHandleStateData& state_data, 
        WindowBasicStyleProperty property) noexcept {

        return WindowBasicStyle::FromHandle(state_data.handle).Has(property);
    }

    static void Set(
        WindowNotCreatedStateData& state_data,
        WindowBasicStyleProperty property,
        bool enable) {

        state_data.basic_style.Set(property, enable);
    }

    static void Set(
        const WindowHandleStateData& state_data,
        WindowBasicStyleProperty property,
        bool enable) {

        auto window_style = WindowBasicStyle::FromHandle(state_data.handle);
        window_style.Set(property, enable);

        SetLastError(0);
        auto previous = SetWindowLong(state_data.handle, GWL_STYLE, window_style.Value());
        if (previous == 0) {
            ZAF_THROW_IF_WIN32_ERROR(GetLastError());
        }
    }
};


template<>
class WindowStyleShim<WindowExtraStyleProperty> {
public:
    static bool Has(
        const WindowNotCreatedStateData& state_data,
        WindowExtraStyleProperty property) noexcept {

        return state_data.extra_style.Has(property);
    }

    static bool Has(
        const WindowHandleStateData& state_data,
        WindowExtraStyleProperty property) noexcept {

        return WindowExtraStyle::FromHandle(state_data.handle).Has(property);
    }

    static void Set(
        WindowNotCreatedStateData& state_data,
        WindowExtraStyleProperty property,
        bool enable) {

        state_data.extra_style.Set(property, enable);
    }

    static void Set(
        const WindowHandleStateData& state_data,
        WindowExtraStyleProperty property,
        bool enable) {

        auto window_style = WindowExtraStyle::FromHandle(state_data.handle);
        window_style.Set(property, enable);

        SetLastError(0);
        auto previous = SetWindowLong(state_data.handle, GWL_EXSTYLE, window_style.Value());
        if (previous == 0) {
            ZAF_THROW_IF_WIN32_ERROR(GetLastError());
        }
    }
};

}