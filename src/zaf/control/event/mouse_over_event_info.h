#pragma once

#include <zaf/base/event/routed_event_info.h>

namespace zaf {

class Control;

namespace internal {

class MouseOverEventSharedState : public RoutedEventSharedState {
public:
    MouseOverEventSharedState(
        const std::shared_ptr<Control>& source,
        const std::shared_ptr<Control>& changed_control);

    const std::shared_ptr<Control>& ChangedControl() const {
        return changed_control_;
    }

private:
    std::shared_ptr<Control> changed_control_;
};

}

/**
Contains information for mouse enter event.
*/
class MouseEnterInfo : public RoutedEventInfo {
public:
    MouseEnterInfo(
        const std::shared_ptr<internal::MouseOverEventSharedState>& shared_state,
        const std::shared_ptr<Control>& sender);

    /**
    The control that mouse left. Can be nullptr.
    */
    const std::shared_ptr<Control>& LeftControl() const {
        return state_->ChangedControl();
    }

private:
    std::shared_ptr<internal::MouseOverEventSharedState> state_;
};


/**
Contains information for mouse leave event.
*/
class MouseLeaveInfo : public RoutedEventInfo {
public:
    MouseLeaveInfo(
        const std::shared_ptr<internal::MouseOverEventSharedState>& shared_state,
        const std::shared_ptr<Control>& sender);

    /**
    The control that mouse entered. Can be nullptr.
    */
    const std::shared_ptr<Control>& EnteredControl() const {
        return state_->ChangedControl();
    }

private:
    std::shared_ptr<internal::MouseOverEventSharedState> state_;
};

}