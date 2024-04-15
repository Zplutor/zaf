#pragma once

#include <functional>
#include <zaf/control/event/keyboard_event_info.h>
#include <zaf/control/event/mouse_event_info.h>

namespace zaf::internal {

using MouseEventInvoker = std::function<void(
    const std::shared_ptr<MouseEventSharedState>& event_state,
    const std::shared_ptr<Control>& sender,
    const Point& position_at_sender
)>;

using KeyboardEventInvoker = std::function<void(
    const std::shared_ptr<KeyboardEventSharedState>& event_state,
    const std::shared_ptr<Control>& sender
)>;


class ControlEventInvokerBinder {
public:
    static const MouseEventInvoker& GetPreMouseEventInvoker(UINT message_id);
    static const MouseEventInvoker& GetMouseEventInvoker(UINT message_id);

    static const KeyboardEventInvoker& GetPreKeyboardEventInvoker(UINT message_id);
    static const KeyboardEventInvoker& GetKeyboardEventInvoker(UINT message_id);

private:
    class MouseEventBinding {
    public:
        MouseEventInvoker pre_event_invoker;
        MouseEventInvoker event_invoker;
    };

    class KeyboardEventBinding {
    public:
        KeyboardEventInvoker pre_event_invoker;
        KeyboardEventInvoker event_invoker;
    };

private:
    static const MouseEventBinding& GetMouseEventBinding(UINT message_id);
    static const KeyboardEventBinding& GetKeyboardEventBinding(UINT message_id);
};

}