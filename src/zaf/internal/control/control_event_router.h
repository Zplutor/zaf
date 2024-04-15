#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/control/control.h>

namespace zaf::internal {

std::vector<std::shared_ptr<Control>> BuildTunnelPath(
    const std::shared_ptr<Control>& event_target);


template<typename E>
class ControlEventRouter : NonCopyableNonMovable {
public:
    using EventInfoStateCreator = std::function<
        std::shared_ptr<E>(const std::shared_ptr<Control>& source)
    >;

    using EventInvoker = std::function<
        void(const std::shared_ptr<E>& event_state, const std::shared_ptr<Control>& sender)
    >;

public:
    void SetEventInfoStateCreator(EventInfoStateCreator creator) {
        event_info_state_creator_ = std::move(creator);
    }

    void SetTunnelingEventInvoker(EventInvoker invoker) {
        tunneling_event_invoker_ = std::move(invoker);
    }

    void SetBubblingEventInvoker(EventInvoker invoker) {
        bubbling_event_invoker_ = std::move(invoker);
    }

    bool Route(const std::shared_ptr<Control>& event_target) const {

        ZAF_EXPECT(event_target);
        ZAF_EXPECT(event_info_state_creator_);
        ZAF_EXPECT(tunneling_event_invoker_);
        ZAF_EXPECT(bubbling_event_invoker_);

        auto tunnel_path = BuildTunnelPath(event_target);
        bool is_handled = TunnelEvent(tunnel_path);

        return BubbleEvent(event_target, is_handled);
    }

private:
    bool TunnelEvent(const std::vector<std::shared_ptr<Control>>& tunnel_path) const {

        if (tunnel_path.empty()) {
            return false;
        }

        //The first control, root control, is the source of tunneling.
        auto event_info_state = event_info_state_creator_(tunnel_path.front());

        for (const auto& each_control : tunnel_path) {
            tunneling_event_invoker_(event_info_state, each_control);
        }

        return event_info_state->IsHandled();
    }

    bool BubbleEvent(
        const std::shared_ptr<Control>& event_target,
        bool is_handled_by_tunneling) const {

        auto event_info_state = event_info_state_creator_(event_target);

        if (is_handled_by_tunneling) {
            event_info_state->MarkAsHandled();
        }

        auto sender = event_target;
        while (sender) {
            bubbling_event_invoker_(event_info_state, sender);
            sender = sender->Parent();
        }

        return event_info_state->IsHandled();
    }

private:
    EventInfoStateCreator event_info_state_creator_;
    EventInvoker tunneling_event_invoker_;
    EventInvoker bubbling_event_invoker_;
};

}