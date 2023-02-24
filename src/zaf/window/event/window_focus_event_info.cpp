#include <zaf/window/event/window_focus_event_info.h>
#include <zaf/window/window.h>

namespace zaf {

WindowFocusGainedInfo::WindowFocusGainedInfo(
    const std::shared_ptr<Window>& source, 
    const zaf::Message& message)
    :
    EventInfo(source),
    message_(message) {

}


WindowFocusLostInfo::WindowFocusLostInfo(
    const std::shared_ptr<Window>& source, 
    const zaf::Message& message)
    :
    EventInfo(source),
    message_(message) {

}

}