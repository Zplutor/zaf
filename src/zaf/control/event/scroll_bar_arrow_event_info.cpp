#include <zaf/control/event/scroll_bar_arrow_event_info.h>
#include <zaf/control/scroll_bar_arrow.h>

namespace zaf::internal {

ScrollBarArrowPressInfo::ScrollBarArrowPressInfo(const std::shared_ptr<ScrollBarArrow>& source) : 
    EventInfo(source) {

}

}