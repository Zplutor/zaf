#include <zaf/control/event/list_control_item_double_click_info.h>
#include <zaf/control/list_control.h>

namespace zaf {

ListControlItemDoubleClickInfo::ListControlItemDoubleClickInfo(
    const std::shared_ptr<ListControl>& source,
    std::size_t item_index)
    :
    EventInfo(source),
    item_index_(item_index) {

}

}