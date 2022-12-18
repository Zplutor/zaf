#include <zaf/control/event/parent_changed_info.h>
#include <zaf/control/control.h>

namespace zaf {

ParentChangedInfo::ParentChangedInfo(
    const std::shared_ptr<Control>& source,
    const std::shared_ptr<Control>& previous_parent)
    :
    EventInfo(source),
    previous_parent_(previous_parent) {

}

}