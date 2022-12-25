#include <zaf/control/event/focus_event_info.h>
#include <zaf/control/control.h>

namespace zaf::internal {

FocusEventInfo::FocusEventInfo(
    const std::shared_ptr<RoutedEventSharedState>& state,
    const std::shared_ptr<Object>& sender) 
    :
    RoutedEventInfo(state, sender) {

}

}