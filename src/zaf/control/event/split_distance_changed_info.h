#pragma once

#include <zaf/base/event/event_info.h>

namespace zaf {

class SplitControl;

class SplitDistanceChangedInfo : public EventInfo {
public:
    SplitDistanceChangedInfo(
        const std::shared_ptr<SplitControl>& source,
        float previous_distance,
        bool is_changed_by_dragging);

    float PreviousDistance() const {
        return previous_distance_;
    }

    bool IsChangedByDragging() const {
        return is_changed_by_dragging_;
    }

private:
    float previous_distance_{};
    bool is_changed_by_dragging_{};
};

}