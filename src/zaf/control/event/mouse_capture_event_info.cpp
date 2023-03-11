#include <zaf/control/event/mouse_capture_event_info.h>
#include <zaf/control/control.h>

namespace zaf::internal {

MouseCaptureEventInfo::MouseCaptureEventInfo(const std::shared_ptr<Control>& source) : 
    EventInfo(source) {

}

}