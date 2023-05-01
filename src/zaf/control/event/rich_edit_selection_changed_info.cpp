#include <zaf/control/event/rich_edit_selection_changed_info.h>
#include <zaf/control/rich_edit.h>

namespace zaf {

RichEditSelectionChangedInfo::RichEditSelectionChangedInfo(
    const std::shared_ptr<RichEdit>& source) 
    : 
    EventInfo(source) {

}

}