#include <zaf/internal/control/keyboard_event_routing.h>
#include <zaf/internal/control/control_event_invoker.h>
#include <zaf/internal/control/control_event_router.h>

namespace zaf::internal {

bool RouteKeyboardEvent(const std::shared_ptr<Control>& event_target, const Message& message) {

    ControlEventRouter<KeyboardEventSharedState> router;

    router.SetEventInfoStateCreator([&message](const std::shared_ptr<Control>& source) {
        return std::make_shared<KeyboardEventSharedState>(source, message);
    });

    router.SetTunnelingEventInvoker(
        ControlEventInvokerBinder::GetPreKeyboardEventInvoker(message.ID()));

    router.SetBubblingEventInvoker(
        ControlEventInvokerBinder::GetKeyboardEventInvoker(message.ID()));

    return router.Route(event_target);
}

}