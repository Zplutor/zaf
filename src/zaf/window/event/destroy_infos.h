#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class Window;

/**
Indicates the reason why a window is destroyed.
*/
enum class DestroyReason {

    /**
    Unspecified reason, typically caused by calling the `DestroyWindow` function directly.
    */
    Unspecified,

    /**
    The window is destroyed by the `WM_CLOSE` message.
    */
    Closed,

    /**
    The window is destroyed because the creation procedure of the window handle failed.
    */
    CreationFailed,
};


class DestroyingInfo : public EventInfo {
public:
    DestroyingInfo(std::shared_ptr<Window> source, DestroyReason reason) noexcept;

    DestroyReason Reason() const noexcept {
        return reason_;
    }

private:
    DestroyReason reason_{ DestroyReason::Unspecified };
};


class DestroyedInfo : public EventInfo {
public:
    DestroyedInfo(
        std::shared_ptr<Window> source, 
        HWND window_handle,
        DestroyReason reason) noexcept;

    HWND WindowHandle() const noexcept {
        return window_handle_;
    }

    DestroyReason Reason() const noexcept {
        return reason_;
    }

private:
    HWND window_handle_{};
    DestroyReason reason_{ DestroyReason::Unspecified };
};

}