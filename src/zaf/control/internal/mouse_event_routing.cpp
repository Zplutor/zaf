#include <zaf/control/internal/mouse_event_routing.h>
#include <zaf/control/internal/control_event_invoker.h>

namespace zaf::internal {
namespace {

class EventTargetInfo {
public:
    std::shared_ptr<Control> control;
    Point position;
};

EventTargetInfo FindEventTargetUpToDown(
    const std::shared_ptr<Control>& control, 
    const Point& position) {

    auto child = control->FindChildAtPosition(position);
    if (!child || !child->IsEnabled()) {
        return EventTargetInfo{ control, position };
    }

    auto position_at_child = control->TranslateToChildPoint(position, child);
    return FindEventTargetUpToDown(child, position_at_child);
}


EventTargetInfo FindEventTargetDownToUp(
    const std::shared_ptr<Control>& control,
    const Point& position) {

    auto current_control = control;
    auto current_position = position;

    while (true) {

        auto parent = current_control->Parent();
        if (!parent) {
            break;
        }

        current_position = current_control->TranslateToParentPoint(current_position);
        current_control = current_control->Parent();

        if (current_control->IsEnabled()) {
            return EventTargetInfo{ current_control, current_position };
        }
    }

    return {};
}


EventTargetInfo FindEventTarget(
    const std::shared_ptr<Control>& control,
    const Point& position) {

    //Find from up to down if the control is enabled.
    if (control->IsEnabled()) {
        return FindEventTargetUpToDown(control, position);
    }
    //Find from down to up if the control is disabled.
    else {
        return FindEventTargetDownToUp(control, position);
    }
}


std::vector<std::shared_ptr<Control>> BuildTunnelPath(
    const std::shared_ptr<Control>& event_target) {

    std::vector<std::shared_ptr<Control>> result;
    auto current = event_target;
    while (current) {
        result.push_back(current);
        current = current->Parent();
    }

    std::reverse(result.begin(), result.end());
    return result;
}


bool TunnelEvent(
    const std::vector<std::shared_ptr<Control>>& tunnel_path,
    const MouseMessage& message) {

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
            position = parent->TranslateToChildPoint(position, each_control);
        }

        //Invoke event handler.
        event_invoker(event_info_state, each_control, position);

        parent = each_control;
    }

    return event_info_state->IsHandled();
}


bool BubbleEvent(
    const std::shared_ptr<Control>& event_target,
    const Point& position,
    const MouseMessage& message,
    bool is_handled_by_tunneling) {

    auto event_info_state = std::make_shared<MouseEventSharedState>(
        event_target,
        message.Inner(),
        position);

    if (is_handled_by_tunneling) {
        event_info_state->MarkAsHandled();
    }

    const auto& event_invoker = ControlEventInvokerBinder::GetMouseEventInvoker(message.ID());

    auto sender = event_target;
    auto position_at_sender = position;
    while (sender) {

        event_invoker(event_info_state, sender, position_at_sender);

        position_at_sender = sender->TranslateToParentPoint(position_at_sender);
        sender = sender->Parent();
    }

    return event_info_state->IsHandled();
}

}


bool RouteMouseEvent(
    const std::shared_ptr<Control>& begin_routing_control, 
    const Point& position_at_begin_routing_control, 
    const MouseMessage& message) {

    auto event_target_info = FindEventTarget(
        begin_routing_control,
        position_at_begin_routing_control);

    if (!event_target_info.control) {
        return false;
    }

    auto tunnel_path = BuildTunnelPath(event_target_info.control);
    bool is_handled = TunnelEvent(tunnel_path, message);

    return BubbleEvent(
        event_target_info.control,
        event_target_info.position,
        message,
        is_handled);
}

}