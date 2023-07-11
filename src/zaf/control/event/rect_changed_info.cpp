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


PositionChangedInfo::PositionChangedInfo(
    const std::shared_ptr<Control>& source,
    const Point& previous_position)
    :
    EventInfo(source),
    previous_position_(previous_position) {

}


SizeChangedInfo::SizeChangedInfo(
    const std::shared_ptr<Control>& source,
    const Size& previous_size)
    :
    EventInfo(source),
    previous_size_(previous_size) {

}

}