#include <zaf/control/event/mouse_cursor_changing_info.h>
#include <zaf/control/control.h>

namespace zaf {
namespace internal {

MouseCursorChangingState::MouseCursorChangingState(
    const std::shared_ptr<Control>& source,
    const zaf::Message& message)
    :
    RoutedEventSharedState(source),
    message_(message) {

}

}

MouseCursorChangingInfo::MouseCursorChangingInfo(
    const std::shared_ptr<internal::MouseCursorChangingState>& state,
    const std::shared_ptr<Control>& sender)
    : 
    RoutedEventInfo(state, sender) {

}

}