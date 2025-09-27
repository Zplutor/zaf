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


class DestroyedInfo : public EventInfo {
public:
    DestroyedInfo(
        std::shared_ptr<Window> source, 
        HWND original_window_handle,
        DestroyReason reason);

    HWND OriginalWindowHandle() const noexcept {
        return original_window_handle_;
    }

    DestroyReason Reason() const noexcept {
        return reason_;
    }

private:
    HWND original_window_handle_{};
    DestroyReason reason_{ DestroyReason::Unspecified };
};

}