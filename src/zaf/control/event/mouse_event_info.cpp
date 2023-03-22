#include <zaf/control/event/mouse_event_info.h>
#include <zaf/control/control.h>

namespace zaf::internal {

MouseEventSharedState::MouseEventSharedState(
    const std::shared_ptr<Control>& source,
    const zaf::Message& message,
    const Point& position_at_source)
    :
    RoutedEventSharedState(source),
    message_(message),
    position_at_source_(position_at_source) {

}

}