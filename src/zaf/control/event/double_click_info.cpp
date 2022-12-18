#include <zaf/control/event/double_click_info.h>
#include <zaf/control/control.h>

namespace zaf {

DoubleClickInfo::DoubleClickInfo(const std::shared_ptr<Control>& source, const Point& position) :
    EventInfo(source),
    position_(position) {

}

}