#include <zaf/internal/control/keyboard_event_routing.h>
#include <zaf/base/error/contract.h>
#include <zaf/internal/control/control_event_invoker.h>
#include <zaf/internal/control/control_event_router.h>

namespace zaf::internal {
namespace {

void TunnelEvent(
    const std::vector<std::shared_ptr<Control>>& tunnel_path,
    const std::shared_ptr<KeyboardEventSharedState>& event_info_state,
    const Message& message) {

    if (tunnel_path.empty()) {
        return;
    }

    const auto& event_invoker =
        ControlEventInvokerBinder::GetPreKeyboardEventInvoker(message.ID());

    for (const auto& each_control : tunnel_path) {
        event_invoker(event_info_state, each_control);
    }
}


void BubbleEvent(
    const std::shared_ptr<Control>& event_target,
    const std::shared_ptr<KeyboardEventSharedState>& event_info_state,
    const Message& message) {

    const auto& event_invoker = ControlEventInvokerBinder::GetKeyboardEventInvoker(message.ID());

    auto sender = event_target;
    while (sender) {

        event_invoker(event_info_state, sender);
        sender = sender->Parent();
    }
}

}

bool RouteKeyboardEvent(const std::shared_ptr<Control>& event_target, const Message& message) {

    ZAF_EXPECT(event_target);

    auto event_info_state = std::make_shared<KeyboardEventSharedState>(event_target, message);

    auto tunnel_path = BuildTunnelPath(event_target);
    TunnelEvent(tunnel_path, event_info_state, message);

    BubbleEvent(event_target, event_info_state, message);

    return event_info_state->IsHandled();
}

}