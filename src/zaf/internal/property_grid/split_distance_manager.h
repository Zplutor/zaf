#pragma once

#include <map>
#include <memory>
#include <zaf/base/non_copyable.h>
#include <zaf/control/split_control.h>
#include <zaf/rx/subjects/subject.h>
#include <zaf/rx/disposable_host.h>

namespace zaf::internal {

class PropertyGridItem;

class ItemSplitDistanceChangedInfo {
public:
    std::shared_ptr<PropertyGridItem> changing_item;
    float new_distance{};
    bool is_changed_by_dragging{};
};

class SplitDistanceManager : rx::DisposableHost, NonCopyableNonMovable {
public:
    SplitDistanceManager();

    //Note that the distance is starting from the left edge of property grid.
    float CurrentDistance() const {
        return current_distance_;
    }

    void UpdateDefaultDistance(float distance);

    const rx::Subject<ItemSplitDistanceChangedInfo>& DistanceChangedSubject() const {
        return distance_changed_subject_;
    }

    float MaxSplitControlX() const {
        return max_split_control_x_;
    }

    void OnSplitControlAdd(const std::shared_ptr<SplitControl>& split_control);
    void OnSplitControlRemove(const std::shared_ptr<SplitControl>& split_control);

    rx::Observable<float> MaxSplitControlXChangedEvent() {
        return max_split_control_x_changed_event_.AsObservable();
    }

private:
    void UpdateMaxSplitControlXOnAdd(const SplitControl& added_split_control);

private:
    rx::Subject<ItemSplitDistanceChangedInfo> distance_changed_subject_;
    float current_distance_{};
    bool is_distance_default_{ true };

    std::map<std::shared_ptr<SplitControl>, std::shared_ptr<rx::Disposable>> split_controls_;
    float max_split_control_x_{};
    rx::Subject<float> max_split_control_x_changed_event_;
};

}