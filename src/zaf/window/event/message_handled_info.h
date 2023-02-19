#pragma once

#include <zaf/base/event/event_info.h>
#include <zaf/window/message/message.h>

namespace zaf {

class Window;

class MessageHandledInfo : public EventInfo {
public:
    MessageHandledInfo(
        const std::shared_ptr<Window>& source,
        const zaf::Message& message,
        LRESULT handle_result);

    const zaf::Message& Message() const {
        return message_;
    }

    LRESULT HandleResult() const {
        return handle_result_;
    }

private:
    zaf::Message message_;
    LRESULT handle_result_{};
};

}