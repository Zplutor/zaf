#include <zaf/control/event/keyboard_event_info.h>
#include <zaf/control/control.h>

namespace zaf::internal {

KeyboardEventSharedState::KeyboardEventSharedState(
    const std::shared_ptr<Control>& source, 
    const zaf::Message& message) 
    :
    RoutedEventSharedState(source),
    message_(message) {

}

}