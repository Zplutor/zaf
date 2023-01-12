#include <zaf/control/event/click_info.h>
#include <zaf/control/clickable_control.h>

namespace zaf {

ClickInfo::ClickInfo(const std::shared_ptr<ClickableControl>& source) : EventInfo(source) {

}

}