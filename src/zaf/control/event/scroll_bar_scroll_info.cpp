#include <zaf/control/event/scroll_bar_scroll_info.h>
#include <zaf/control/scroll_bar.h>

namespace zaf {

ScrollBarScrollInfo::ScrollBarScrollInfo(const std::shared_ptr<ScrollBar>& source) :
    EventInfo(source) {

}

}