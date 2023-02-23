#pragma once

#include <zaf/base/event/event_info.h>
#include <zaf/window/message/show_window_message.h>

namespace zaf {

class Window;

namespace internal {

class ShowWindowEventInfo : public EventInfo {
public:
    explicit ShowWindowEventInfo(
        const std::shared_ptr<Window>& source,
        const ShowWindowMessage& message);

    ShowWindowMessage Message() const {
        return ShowWindowMessage{ message_ };
    }

private:
    zaf::Message message_;
};

}

using ShowInfo = internal::ShowWindowEventInfo;
using HideInfo = internal::ShowWindowEventInfo;

}