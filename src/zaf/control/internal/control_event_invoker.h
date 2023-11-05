#pragma once

#include <functional>
#include <zaf/control/event/mouse_event_info.h>

namespace zaf::internal {

using MouseEventInvoker = std::function<void(
    const std::shared_ptr<MouseEventSharedState>& event_state,
    const std::shared_ptr<Control>& sender,
    const Point& position_at_sender
)>;


class ControlEventInvokerBinder {
public:
    static const MouseEventInvoker& GetPreMouseEventInvoker(UINT message_id);
    static const MouseEventInvoker& GetMouseEventInvoker(UINT message_id);

private:
    class MouseEventBinding {
    public:
        MouseEventInvoker pre_event_invoker;
        MouseEventInvoker event_invoker;
    };

private:
    static const MouseEventBinding& GetMouseEventBinding(UINT message_id);
};

}