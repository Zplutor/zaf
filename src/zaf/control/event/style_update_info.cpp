#include <zaf/control/event/style_update_info.h>
#include <zaf/control/control.h>

namespace zaf {

StyleUpdateInfo::StyleUpdateInfo(const std::shared_ptr<Control>& control) : 
    EventInfo(control) {

}

}