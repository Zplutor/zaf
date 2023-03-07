#include <zaf/control/event/list_control_selection_changed_info.h>
#include <zaf/control/list_control.h>

namespace zaf {

ListControlSelectionChangedInfo::ListControlSelectionChangedInfo(
    const std::shared_ptr<ListControl>& source)
    : 
    EventInfo(source) {

}

}