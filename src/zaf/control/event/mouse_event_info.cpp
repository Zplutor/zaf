#include <zaf/control/event/mouse_event_info.h>
#include <zaf/control/control.h>

namespace zaf::internal {

MouseOverInfo::MouseOverInfo(const std::shared_ptr<Control>& source) :
    RoutedEventInfo(source) {

}

}