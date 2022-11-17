#include <zaf/control/property_grid/internal/property_grid_split_distance_manager.h>

namespace zaf::internal {

PropertyGridSplitDistanceManager::PropertyGridSplitDistanceManager() {

    Subscriptions() += distance_change_subject_.GetObservable().Subscribe(
        [this](const PropertyGridSplitDistanceChangeInfo& event_info) {
    
        current_distance_ = event_info.new_distance;
    });
}

}