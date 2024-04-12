#pragma once

#include <zaf/base/event/routed_event_info.h>

namespace zaf {

class Control;

namespace internal {

class FocusEventSharedState : public RoutedEventSharedState {
public:
    FocusEventSharedState(
        std::shared_ptr<Control> source,
        std::shared_ptr<Control> changing_control);

    const std::shared_ptr<Control>& ChangingControl() const {
        return changing_control_;
    }

private:
    std::shared_ptr<Control> changing_control_;
};


template<int>
class TemplateFocusGainedInfo : public RoutedEventInfo {
public:
    TemplateFocusGainedInfo(
        const std::shared_ptr<internal::FocusEventSharedState>& state,
        const std::shared_ptr<Control>& sender)
        :
        RoutedEventInfo(state, sender),
        state_(state) {

    }

    const std::shared_ptr<Control>& LostFocusControl() const {
        return state_->ChangingControl();
    }

private:
    std::shared_ptr<internal::FocusEventSharedState> state_;
};



template<int>
class TemplateFocusLostInfo : public RoutedEventInfo {
public:
    TemplateFocusLostInfo(
        const std::shared_ptr<internal::FocusEventSharedState>& state,
        const std::shared_ptr<Control>& sender)
        :
        RoutedEventInfo(state, sender),
        state_(state) {

    }

    const std::shared_ptr<Control>& GainedFocusControl() const {
        return state_->ChangingControl();
    }

private:
    std::shared_ptr<internal::FocusEventSharedState> state_;
};

}
}