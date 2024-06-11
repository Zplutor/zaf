#include <zaf/control/event/visual_state_update_info.h>
#include <zaf/control/control.h>

namespace zaf {

VisualStateUpdateInfo::VisualStateUpdateInfo(const std::shared_ptr<Control>& control) : 
    EventInfo(control) {

}

}