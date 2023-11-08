#include <zaf/control/event/is_selected_changed_info.h>
#include <zaf/control/control.h>

namespace zaf {

IsSelectedChangedInfo::IsSelectedChangedInfo(std::shared_ptr<Control> source) :
    EventInfo(std::move(source)) {

}

}