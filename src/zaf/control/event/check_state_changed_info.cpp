#include <zaf/control/event/check_state_changed_info.h>
#include <zaf/control/check_box.h>

namespace zaf {

CheckStateChangedInfo::CheckStateChangedInfo(const std::shared_ptr<CheckBox>& source) : 
    EventInfo(source) {

}

}