#include <zaf/control/event/window_changed_info.h>
#include <zaf/control/control.h>

namespace zaf {

WindowChangedInfo::WindowChangedInfo(
    const std::shared_ptr<Control>& source,
    const std::shared_ptr<Window>& previous_window) 
    :
    EventInfo(source),
    previous_window_(previous_window) {

}

}