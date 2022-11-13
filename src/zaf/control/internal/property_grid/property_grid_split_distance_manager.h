#pragma once

#include <memory>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/subject.h>
#include <zaf/rx/subscription_host.h>

namespace zaf::internal {

class PropertyGridItem;

class PropertyGridSplitDistanceChangeInfo {
public:
    std::shared_ptr<PropertyGridItem> changing_item;
    float new_distance{};
};

class PropertyGridSplitDistanceManager : SubscriptionHost, NonCopyable {
public:
    PropertyGridSplitDistanceManager();

    float CurrentDistance() const {
        return current_distance_;
    }

    const zaf::Subject<PropertyGridSplitDistanceChangeInfo>& DistanceChangeSubject() const {
        return distance_change_subject_;
    }

private:
    zaf::Subject<PropertyGridSplitDistanceChangeInfo> distance_change_subject_;
    float current_distance_{ 150 };
};

}