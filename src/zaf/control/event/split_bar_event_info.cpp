#include <zaf/control/event/split_bar_event_info.h>
#include <zaf/control/split_control.h>

namespace zaf::internal {

SplitBarEventInfo::SplitBarEventInfo(const std::shared_ptr<SplitBar>& source) : EventInfo(source) {

}

}