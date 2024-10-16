#include <zaf/control/event/list_control_event_infos.h>
#include <zaf/control/list_control.h>

namespace zaf {

ListControlSelectionChangedInfo::ListControlSelectionChangedInfo(
    const std::shared_ptr<ListControl>& source)
    : 
    EventInfo(source) {

}


ListControlItemDoubleClickInfo::ListControlItemDoubleClickInfo(
    const std::shared_ptr<ListControl>& source,
    std::size_t item_index)
    :
    EventInfo(source),
    item_index_(item_index) {

}


ListControlContextMenuInfo::ListControlContextMenuInfo(
    std::shared_ptr<ListControl> source, 
    std::optional<std::size_t> item_index, 
    std::shared_ptr<Object> item_data)
    :
    EventInfo(std::move(source)),
    item_index_(item_index_),
    item_data_(std::move(item_data)),
    menu_(std::make_shared<std::shared_ptr<PopupMenu>>()) {

}

}