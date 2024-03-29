#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class Window;

class DestroyedInfo : public EventInfo {
public:
    explicit DestroyedInfo(
        const std::shared_ptr<Window>& source, 
        HWND original_window_handle);

    HWND OriginalWindowHandle() const {
        return original_window_handle_;
    }

private:
    HWND original_window_handle_{};
};

}