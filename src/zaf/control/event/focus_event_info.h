#pragma once

#include <zaf/base/event/routed_event_info.h>

namespace zaf {

class Control;

namespace internal {

class FocusEventSharedState : public RoutedEventSharedState {
public:
    FocusEventSharedState(
        const std::shared_ptr<Control>& source,
        const std::shared_ptr<Control>& changing_control);

    const std::shared_ptr<Control>& ChangingControl() const {
        return changing_control_;
    }

private:
    std::shared_ptr<Control> changing_control_;
};

}

class FocusGainedInfo : public RoutedEventInfo {
public:
    FocusGainedInfo(
        const std::shared_ptr<internal::FocusEventSharedState>& state,
        const std::shared_ptr<Control>& sender);

    const std::shared_ptr<Control>& LostFocusControl() const {
        return state_->ChangingControl();
    }

private:
    std::shared_ptr<internal::FocusEventSharedState> state_;
};


class FocusLostInfo : public RoutedEventInfo {
public:
    FocusLostInfo(
        const std::shared_ptr<internal::FocusEventSharedState>& state,
        const std::shared_ptr<Control>& sender);

    const std::shared_ptr<Control>& GainedFocusControl() const {
        return state_->ChangingControl();
    }

private:
    std::shared_ptr<internal::FocusEventSharedState> state_;
};

}