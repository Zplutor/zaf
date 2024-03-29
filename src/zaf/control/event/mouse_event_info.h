#pragma once

#include <zaf/base/event/routed_event_info.h>
#include <zaf/graphic/point.h>
#include <zaf/window/message/message.h>
#include <zaf/window/message/mouse_message.h>

namespace zaf {

class Control;

namespace internal {

class MouseEventSharedState : public RoutedEventSharedState {
public:
    MouseEventSharedState(
        const std::shared_ptr<Control>& source,
        const zaf::Message& message,
        const Point& position_at_source);

    const zaf::Message& Message() const {
        return message_;
    }

    const Point& PositionAtSource() const {
        return position_at_source_;
    }

private:
    zaf::Message message_;
    Point position_at_source_;
};


template<typename MessageShim, int>
class MouseEventInfo : public RoutedEventInfo {
public:
    MouseEventInfo(
        const std::shared_ptr<MouseEventSharedState>& state,
        const std::shared_ptr<Control>& sender,
        const Point& position_at_sender)
        :
        RoutedEventInfo(state, sender),
        state_(state),
        position_at_sender_(position_at_sender) {

    }

    MessageShim Message() const {
        return MessageShim{ state_->Message() };
    }

    const Point& PositionAtSource() const {
        return state_->PositionAtSource();
    }

    const Point& PositionAtSender() const {
        return position_at_sender_;
    }

private:
    std::shared_ptr<MouseEventSharedState> state_;
    Point position_at_sender_;
};

}

using PreMouseMoveInfo = internal::MouseEventInfo<MouseMessage, __LINE__>;
using MouseMoveInfo = internal::MouseEventInfo<MouseMessage, __LINE__>;

using PreMouseDownInfo = internal::MouseEventInfo<MouseMessage, __LINE__>;
using MouseDownInfo = internal::MouseEventInfo<MouseMessage, __LINE__>;

using PreMouseUpInfo = internal::MouseEventInfo<MouseMessage, __LINE__>;
using MouseUpInfo = internal::MouseEventInfo<MouseMessage, __LINE__>;

using PreMouseWheelInfo = internal::MouseEventInfo<MouseWheelMessage, __LINE__>;
using MouseWheelInfo = internal::MouseEventInfo<MouseWheelMessage, __LINE__>;

}