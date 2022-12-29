#include <zaf/control/event/focus_event_info.h>
#include <zaf/control/control.h>

namespace zaf {
namespace internal {

FocusEventSharedState::FocusEventSharedState(
    const std::shared_ptr<Control>& source,
    const std::shared_ptr<Control>& changing_control)
    :
    RoutedEventSharedState(source),
    changing_control_(changing_control) {

}

}


FocusGainedInfo::FocusGainedInfo(
    const std::shared_ptr<internal::FocusEventSharedState>& state,
    const std::shared_ptr<Control>& sender) 
    :
    RoutedEventInfo(state, sender),
    state_(state) {

}


FocusLostInfo::FocusLostInfo(
    const std::shared_ptr<internal::FocusEventSharedState>& state,
    const std::shared_ptr<Control>& sender)
    :
    RoutedEventInfo(state, sender),
    state_(state) {

}

}