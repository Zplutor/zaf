#include <zaf/window/event/activate_event_info.h>
#include <zaf/window/window.h>

namespace zaf::internal {

ActivateEventInfo::ActivateEventInfo(
    const std::shared_ptr<Window>& source,
    const ActivateMessage& message)
    : 
    EventInfo(source),
    message_(message.Inner()) {

}

}