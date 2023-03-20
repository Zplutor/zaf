#include <zaf/control/event/sub_menu_popup_event.h>
#include <zaf/control/menu_item.h>

namespace zaf::internal {

SubMenuEventInfo::SubMenuEventInfo(const std::shared_ptr<MenuItem>& source) : EventInfo(source) {

}

}