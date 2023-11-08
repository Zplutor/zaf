#include <zaf/control/event/is_enabled_changed_info.h>
#include <zaf/control/control.h>

namespace zaf {

IsEnabledChangedInfo::IsEnabledChangedInfo(std::shared_ptr<Control> source) :
    EventInfo(std::move(source)) {

}

}