#include <zaf/control/property_grid/internal/split_distance_manager.h>

namespace zaf::property_grid::internal {

SplitDistanceManager::SplitDistanceManager() {

    Subscriptions() += distance_change_subject_.GetObservable().Subscribe(
        [this](const SplitDistanceChangeInfo& event_info) {
    
        current_distance_ = event_info.new_distance;
    });
}

}