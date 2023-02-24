#pragma once

#include <zaf/base/event/event_info.h>
#include <zaf/window/message/focus_message.h>

namespace zaf {

class Window;

class WindowFocusGainedInfo : public EventInfo {
public:
    WindowFocusGainedInfo(const std::shared_ptr<Window>& source, const zaf::Message& message);

    SetFocusMessage Message() const {
        return SetFocusMessage{ message_ };
    }

private:
    zaf::Message message_;
};


class WindowFocusLostInfo : public EventInfo {
public:
    WindowFocusLostInfo(const std::shared_ptr<Window>& source, const zaf::Message& message);

    KillFocusMessage Message() const {
        return KillFocusMessage{ message_ };
    }

private:
    zaf::Message message_;
};

}