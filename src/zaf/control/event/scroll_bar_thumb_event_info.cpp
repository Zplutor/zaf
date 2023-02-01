#include <zaf/control/event/scroll_bar_thumb_event_info.h>
#include <zaf/control/scroll_bar_thumb.h>

namespace zaf::internal {

ScrollBarThumbEventInfo::ScrollBarThumbEventInfo(const std::shared_ptr<ScrollBarThumb>& source) : 
    EventInfo(source) {

}

}