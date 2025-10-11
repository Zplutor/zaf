#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class Window;

class HandleCreatingInfo : public EventInfo {
public:
    HandleCreatingInfo(std::shared_ptr<Window> source, HWND window_handle);

    HWND WindowHandle() const noexcept {
        return window_handle_;
    }

private:
    HWND window_handle_{};
};


class HandleCreatedInfo : public EventInfo {
public:
    explicit HandleCreatedInfo(const std::shared_ptr<Window>& source);
};

}