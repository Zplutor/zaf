#include <zaf/control/internal/mouse_event_routing.h>
#include <zaf/control/internal/control_event_invoker.h>

namespace zaf::internal {
namespace {

std::shared_ptr<Control> FindEventTargetUpToDown(
    const std::shared_ptr<Control>& control,
    const Point& position) {

    auto child = control->FindChildAtPosition(position);
    if (!child || !child->IsEnabled()) {
        return control;
    }

    auto position_at_child = control->TranslateToChildPoint(position, child);
    return FindEventTargetUpToDown(child, position_at_child);
}


std::shared_ptr<Control> FindEventTargetDownToUp(const std::shared_ptr<Control>& control) {

    auto current = control;
    while (current = current->Parent()) {

        if (current->IsEnabled()) {
            return current;
        }
    }
    return nullptr;
}


std::shared_ptr<Control> FindEventTarget(
    const std::shared_ptr<Control>& control,
    const Point& position) {

    //Find from up to down if the control is enabled.
    if (control->IsEnabled()) {
        return FindEventTargetUpToDown(control, position);
    }
    //Find from down to up if the control is disabled.
    else {
        return FindEventTargetDownToUp(control);
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


void TunnelEvent(
    const std::vector<std::shared_ptr<Control>>& tunnel_path,
    const MouseMessage& message) {

    if (tunnel_path.empty()) {
        return;
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
}


void BubbleEvent();

}


bool RouteMouseEvent(
    const std::shared_ptr<Control>& begin_routing_control, 
    const Point& position_at_begin_routing_control, 
    const MouseMessage& message) {

    auto event_target = FindEventTarget(begin_routing_control, position_at_begin_routing_control);
    if (!event_target) {
        return false;
    }

    auto tunnel_path = BuildTunnelPath(event_target);
    TunnelEvent(tunnel_path, message);
    return true;
}

}