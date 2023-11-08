#include <zaf/control/event/is_visible_changed_info.h>
#include <zaf/control/control.h>

namespace zaf {

IsVisibleChangedInfo::IsVisibleChangedInfo(std::shared_ptr<Control> source) :
    EventInfo(std::move(source)) {

}

}