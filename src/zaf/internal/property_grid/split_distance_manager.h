#pragma once

#include <map>
#include <memory>
#include <zaf/base/non_copyable.h>
#include <zaf/control/split_control.h>
#include <zaf/rx/subject.h>
#include <zaf/rx/subscription_host.h>

namespace zaf::property_grid::internal {

class Item;

class ItemSplitDistanceChangedInfo {
public:
    std::shared_ptr<Item> changing_item;
    float new_distance{};
    bool is_changed_by_dragging{};
};

class SplitDistanceManager : SubscriptionHost, NonCopyableNonMovable {
public:
    SplitDistanceManager();

    //Note that the distance is starting from the left edge of property grid.
    float CurrentDistance() const {
        return current_distance_;
    }

    void UpdateDefaultDistance(float distance);

    const Subject<ItemSplitDistanceChangedInfo>& DistanceChangedSubject() const {
        return distance_changed_subject_;
    }

    float MaxSplitControlX() const {
        return max_split_control_x_;
    }

    void OnSplitControlAdd(const std::shared_ptr<SplitControl>& split_control);
    void OnSplitControlRemove(const std::shared_ptr<SplitControl>& split_control);

    Observable<float> MaxSplitControlXChangedEvent() {
        return max_split_control_x_changed_event_.AsObservable();
    }

private:
    void UpdateMaxSplitControlXOnAdd(const SplitControl& added_split_control);

private:
    Subject<ItemSplitDistanceChangedInfo> distance_changed_subject_;
    float current_distance_{};
    bool is_distance_default_{ true };

    std::map<std::shared_ptr<SplitControl>, Subscription> split_controls_;
    float max_split_control_x_{};
    Subject<float> max_split_control_x_changed_event_;
};

}