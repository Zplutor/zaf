#include <zaf/control/internal/mouse_event_routing.h>
#include <zaf/control/internal/control_event_invoker.h>
#include <zaf/control/internal/event_routing_utility.h>

namespace zaf::internal {
namespace {

MouseEventTargetInfo FindEventTargetUpToDown(
    const std::shared_ptr<Control>& control, 
    const Point& position) {

    auto child = control->FindChildAtPosition(position);
    if (!child || !child->IsEnabled()) {
        return MouseEventTargetInfo{ control, position };
    }

    auto position_at_child = control->TranslatePositionToChild(position, *child);
    return FindEventTargetUpToDown(child, position_at_child);
}


MouseEventTargetInfo FindEventTargetDownToUp(
    const std::shared_ptr<Control>& control,
    const Point& position) {

    auto current_control = control;
    auto current_position = position;

    while (true) {

        auto parent = current_control->Parent();
        if (!parent) {
            break;
        }

        current_position = current_control->TranslatePositionToParent(current_position);
        current_control = current_control->Parent();

        if (current_control->IsEnabled()) {
            return MouseEventTargetInfo{ current_control, current_position };
        }
    }

    return {};
}

}


MouseEventTargetInfo FindMouseEventTarget(
    const std::shared_ptr<Control>& begin_control,
    const Point& position_at_begin_control) {

    //Find from up to down if the control is enabled.
    if (begin_control->IsEnabled()) {
        return FindEventTargetUpToDown(begin_control, position_at_begin_control);
    }
    //Find from down to up if the control is disabled.
    else {
        return FindEventTargetDownToUp(begin_control, position_at_begin_control);
    }
}


bool TunnelMouseEvent(
    const std::shared_ptr<Control>& event_target,
    const MouseMessage& message) {

    auto tunnel_path = BuildTunnelPath(event_target);
    if (tunnel_path.empty()) {
        return false;
    }

    //Event tunneling begins from root control, whose coordinate space is the same as the window.
    //Thus we can use the position of mouse message as the first position.
    auto position = message.MousePosition();

    auto event_info_state = std::make_shared<MouseEventSharedState>(
        tunnel_path.front(), //The first control, root control, is the source of tunneling.
        message.Inner(),
        position);

    const auto& event_invoker = ControlEventInvokerBinder::GetPreMouseEventInvoker(message.ID());

    std::shared_ptr<Control> parent;
    for (const auto& each_control : tunnel_path) {

        if (parent) {
            position = parent->TranslatePositionToChild(position, *each_control);
        }

        //Invoke event handler.
        event_invoker(event_info_state, each_control, position);

        parent = each_control;
    }

    return event_info_state->IsHandled();
}


bool BubbleMouseEvent(
    const std::shared_ptr<Control>& event_target,
    const Point& position_at_event_target, 
    const MouseMessage& message,
    bool is_handled_by_tunneling) {

    auto event_info_state = std::make_shared<MouseEventSharedState>(
        event_target,
        message.Inner(),
        position_at_event_target);

    if (is_handled_by_tunneling) {
        event_info_state->MarkAsHandled();
    }

    const auto& event_invoker = ControlEventInvokerBinder::GetMouseEventInvoker(message.ID());

    auto sender = event_target;
    auto position_at_sender = position_at_event_target;
    while (true) {

        event_invoker(event_info_state, sender, position_at_sender);

        auto parent = sender->Parent();
        if (!parent) {
            break;
        }

        position_at_sender = sender->TranslatePositionToParent(position_at_sender);
        sender = parent;
    }

    return event_info_state->IsHandled();
}

}