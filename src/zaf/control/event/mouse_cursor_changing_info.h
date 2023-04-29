#pragma once

#include <zaf/base/event/routed_event_info.h>
#include <zaf/window/message/set_cursor_message.h>

namespace zaf {

class Control;

namespace internal {

class MouseCursorChangingState : public RoutedEventSharedState {
public:
    MouseCursorChangingState(
        const std::shared_ptr<Control>& source,
        const zaf::Message& message);

    const zaf::Message& Message() const {
        return message_;
    }

private:
    zaf::Message message_;
};

}

class MouseCursorChangingInfo : public RoutedEventInfo {
public:
    MouseCursorChangingInfo(
        const std::shared_ptr<internal::MouseCursorChangingState>& state,
        const std::shared_ptr<Control>& sender);

    const SetCursorMessage& Message() const {
        return SetCursorMessage{ state_->Message() };
    }

private:
    std::shared_ptr<internal::MouseCursorChangingState> state_;
};

}