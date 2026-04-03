#pragma once

#include <optional>
#include <zaf/base/event/event_info.h>
#include <zaf/window/message/message.h>

namespace zaf {
namespace internal {

class MessageHandlingSharedState {
public:
    explicit MessageHandlingSharedState(const Message& m) : message(m) {

    }

    Message message;
    std::optional<LRESULT> handle_result;
};

}

class Window;

class MessageHandlingInfo : public EventInfo {
public:
    MessageHandlingInfo(
        const std::shared_ptr<Window>& source,
        const Message& message);

    const Message& Message() const {
        return state_->message;
    }

    bool IsHandled() const {
        return state_->handle_result.has_value();
    }

    std::optional<LRESULT> HandleResult() const {
        return state_->handle_result;
    }

    void MarkAsHandled(LRESULT handle_result) const {
        state_->handle_result = handle_result;
    }

private:
    std::shared_ptr<internal::MessageHandlingSharedState> state_;
};

}