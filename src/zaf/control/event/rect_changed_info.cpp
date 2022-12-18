#include <zaf/control/event/rect_changed_info.h>
#include <zaf/control/control.h>

namespace zaf {

RectChangedInfo::RectChangedInfo(
    const std::shared_ptr<Control>& source, 
    const Rect& previous_rect) 
    :
    EventInfo(source),
    previous_rect_(previous_rect) {

}

}