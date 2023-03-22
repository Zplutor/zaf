#include <zaf/control/event/mouse_over_event_info.h>
#include <zaf/control/control.h>

namespace zaf {
namespace internal {

MouseOverEventSharedState::MouseOverEventSharedState(
    const std::shared_ptr<Control>& source,
    const std::shared_ptr<Control>& changed_control)
    :
    RoutedEventSharedState(source),
    changed_control_(changed_control) {

}

}


MouseEnterInfo::MouseEnterInfo(
    const std::shared_ptr<internal::MouseOverEventSharedState>& shared_state,
    const std::shared_ptr<Control>& sender)
    :
    RoutedEventInfo(shared_state, sender),
    state_(shared_state) {

}


MouseLeaveInfo::MouseLeaveInfo(
    const std::shared_ptr<internal::MouseOverEventSharedState>& shared_state,
    const std::shared_ptr<Control>& sender)
    :
    RoutedEventInfo(shared_state, sender),
    state_(shared_state) {

}

}