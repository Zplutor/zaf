#include <zaf/window/event/mouse_capture_control_changed_info.h>
#include <zaf/window/window.h>

namespace zaf {

MouseCaptureControlChangedInfo::MouseCaptureControlChangedInfo(
    const std::shared_ptr<Window>& source,
    const std::shared_ptr<Control>& previous_control) 
    :
    EventInfo(source),
    previous_control_(previous_control) {

}

}