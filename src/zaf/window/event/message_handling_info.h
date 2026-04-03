#pragma once

#include <optional>
#include <zaf/base/event/event_info.h>
#include <zaf/window/message/message.h>

namespace zaf {
namespace internal {

class MessageHandlingSharedState {
public:
    MessageHandlingSharedState(const Message& message, bool can_be_handled) noexcept : 
        message(message),
        can_be_handled(can_be_handled) {

    }

    Message message;
    bool can_be_handled{};
    std::optional<LRESULT> handle_result;
};

}

class Window;

class MessageHandlingInfo : public EventInfo {
public:
    MessageHandlingInfo(
        const std::shared_ptr<Window>& source,
        const Message& message,
        bool can_be_handled);

    const Message& Message() const noexcept {
        return state_->message;
    }

    /**
    Indicates whether the message can be handled.

    @return
        `true` if the message can be handled, `false` otherwise.

    @details
        The following messages cannot be handled:
        - `WM_NCCREATE`
        - `WM_CREATE`
        - `WM_DESTROY`
        - `WM_NCDESTROY`
    */
    bool CanBeHandled() const noexcept {
        return state_->can_be_handled;
    }

    bool IsHandled() const noexcept {
        return state_->handle_result.has_value();
    }

    std::optional<LRESULT> HandleResult() const noexcept {
        return state_->handle_result;
    }

    /**
    Marks the message as handled, and sets the result of handling the message.

    @param handle_result
        The result of handling the message, which will be returned to the caller of the window
        procedure.

    @throw zaf::InvalidOperationError
        Thrown if the message cannot be handled.

    @details
        If a message is marked as handled, it is prevented from being handled by the window. Some
        critical messages cannot be marked as handled, as they are essential for the window.

        @warning
        Be careful when marking a message as handled, as it may cause unexpected behavior to the 
        window if the message affects the window's state.

    @see zaf::MessageHandlingInfo::CanBeHandled()
    */
    void MarkAsHandled(LRESULT handle_result) const;

private:
    std::shared_ptr<internal::MessageHandlingSharedState> state_;
};

}