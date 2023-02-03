#include <zaf/control/event/split_distance_changed_info.h>
#include <zaf/control/split_control.h>

namespace zaf {

SplitDistanceChangedInfo::SplitDistanceChangedInfo(
    const std::shared_ptr<SplitControl>& source,
    float previous_distance,
    bool is_changed_by_dragging)
    :
    EventInfo(source),
    previous_distance_(previous_distance),
    is_changed_by_dragging_(is_changed_by_dragging) {

}

}