#pragma once

#include <zaf/window/message/message.h>

namespace zaf {

class Window;

class WindowEventInfo {
public:
    WindowEventInfo(const std::shared_ptr<Window>& window) : window_(window) {

    }

    const std::shared_ptr<Window>& Window() const {
        return window_;
    }

private:
    std::shared_ptr<zaf::Window> window_;
};


class WindowCloseInfo : public WindowEventInfo {
public:
    using WindowEventInfo::WindowEventInfo;

    bool CanClose() const {
        return can_close_;
    }

    void SetCanClose(bool can_close) {
        can_close_ = can_close;
    }

private:
    bool can_close_{ true };
};

}