#pragma once

#include <zaf/base/event/event_info.h>
#include <zaf/window/message/show_window_message.h>

namespace zaf {

class Window;

class ShowInfo : public EventInfo {
public:
    explicit ShowInfo(
        const std::shared_ptr<Window>& source,
        const ShowWindowMessage& message);

    ShowWindowMessage Message() const {
        return ShowWindowMessage{ message_ };
    }

private:
    zaf::Message message_;
};

}