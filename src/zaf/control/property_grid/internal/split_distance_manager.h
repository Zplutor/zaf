#pragma once

#include <memory>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/subject.h>
#include <zaf/rx/subscription_host.h>

namespace zaf::property_grid::internal {

class Item;

class SplitDistanceChangeInfo {
public:
    std::shared_ptr<Item> changing_item;
    float new_distance{};
};

class SplitDistanceManager : SubscriptionHost, NonCopyable {
public:
    SplitDistanceManager();

    //Note that the distance is starting from the left edge of property grid.
    float CurrentDistance() const {
        return current_distance_;
    }

    const zaf::Subject<SplitDistanceChangeInfo>& DistanceChangeSubject() const {
        return distance_change_subject_;
    }

private:
    zaf::Subject<SplitDistanceChangeInfo> distance_change_subject_;
    float current_distance_{ 150 };
};

}