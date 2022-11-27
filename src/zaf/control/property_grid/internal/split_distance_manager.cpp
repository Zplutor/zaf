#include <zaf/control/property_grid/internal/split_distance_manager.h>

namespace zaf::property_grid::internal {

SplitDistanceManager::SplitDistanceManager() {

    Subscriptions() += distance_change_subject_.GetObservable().Subscribe(
        [this](const SplitDistanceChangeInfo& event_info) {
    
        if (is_distance_default_ && event_info.new_distance == current_distance_) {
            return;
        }

        current_distance_ = event_info.new_distance;
        is_distance_default_ = false;
    });
}


void SplitDistanceManager::UpdateDefaultDistance(float distance) {

    if (is_distance_default_) {
        current_distance_ = distance;
    }
}

}