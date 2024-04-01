#include <zaf/control/internal/keyboard_event_routing.h>
#include <zaf/base/error/contract.h>
#include <zaf/control/internal/control_event_invoker.h>
#include <zaf/control/internal/event_routing_utility.h>

namespace zaf::internal {
namespace {

bool TunnelEvent(
    const std::vector<std::shared_ptr<Control>>& tunnel_path,
    const Message& message) {

    if (tunnel_path.empty()) {
        return false;
    }

    auto event_info_state = std::make_shared<KeyboardEventSharedState>(
        tunnel_path.front(), //The first control, root control, is the source of tunneling.
        message);

    const auto& event_invoker =
        ControlEventInvokerBinder::GetPreKeyboardEventInvoker(message.ID());

    for (const auto& each_control : tunnel_path) {
        event_invoker(event_info_state, each_control);
    }

    return event_info_state->IsHandled();
}


bool BubbleEvent(
    const std::shared_ptr<Control>& event_target,
    const Message& message,
    bool is_handled_by_tunneling) {

    auto event_info_state = std::make_shared<KeyboardEventSharedState>(
        event_target,
        message);

    if (is_handled_by_tunneling) {
        event_info_state->MarkAsHandled();
    }

    const auto& event_invoker = ControlEventInvokerBinder::GetKeyboardEventInvoker(message.ID());

    auto sender = event_target;
    while (sender) {
        event_invoker(event_info_state, sender);
        sender = sender->Parent();
    }

    return event_info_state->IsHandled();
}

}

bool RouteKeyboardEvent(const std::shared_ptr<Control>& event_target, const Message& message) {

    ZAF_EXPECT(event_target);

    auto tunnel_path = BuildTunnelPath(event_target);
    bool is_handled = TunnelEvent(tunnel_path, message);

    return BubbleEvent(event_target, message, is_handled);
}

}