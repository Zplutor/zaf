#pragma once

#include <zaf/base/event/event_info.h>
#include <zaf/window/message/activate_message.h>

namespace zaf {

class Window;

namespace internal {

class ActivateEventInfo : public EventInfo {
public:
    ActivateEventInfo(const std::shared_ptr<Window>& source, const ActivateMessage& message);

    ActivateMessage Message() const {
        return ActivateMessage{ message_ };
    }

private:
    zaf::Message message_;
};

}

using ActivatedInfo = internal::ActivateEventInfo;
using DeactivatedInfo = internal::ActivateEventInfo;

}