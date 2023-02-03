#include <zaf/control/property_grid/internal/split_distance_manager.h>
#include <zaf/base/as.h>

namespace zaf::property_grid::internal {

SplitDistanceManager::SplitDistanceManager() {

    Subscriptions() += distance_changed_subject_.GetObservable().Subscribe(
        [this](const ItemSplitDistanceChangedInfo& event_info) {
    
        if (!event_info.changing_item) {
            return;
        }

        if (is_distance_default_ && !event_info.is_changed_by_dragging) {
            return;
        }

        current_distance_ = event_info.new_distance;
        is_distance_default_ = false;
    });
}


void SplitDistanceManager::UpdateDefaultDistance(float distance) {

    if (!is_distance_default_) {
        return;
    }

    current_distance_ = distance;

    ItemSplitDistanceChangedInfo event_info;
    event_info.new_distance = current_distance_;
    distance_changed_subject_.GetObserver().OnNext(event_info);
}


void SplitDistanceManager::OnSplitControlAdd(const std::shared_ptr<SplitControl>& split_control) {

    UpdateMaxSplitControlXOnAdd(*split_control);

    auto subscription = split_control->RectChangedEvent().Subscribe(
        [this](const RectChangedInfo& event_info) {

        auto& split_control = As<SplitControl>(*event_info.Source());
        if (split_control.Rect().Left() == event_info.PreviousRect().Left()) {
            return;
        }

        UpdateMaxSplitControlXOnAdd(split_control);
    });

    split_controls_[split_control] = subscription;
}


void SplitDistanceManager::UpdateMaxSplitControlXOnAdd(const SplitControl& added_split_control) {

    if (max_split_control_x_ >= added_split_control.X()) {
        return;
    }

    max_split_control_x_ = added_split_control.X();
    max_split_control_x_changed_event_.GetObserver().OnNext(max_split_control_x_);
}


void SplitDistanceManager::OnSplitControlRemove(
    const std::shared_ptr<SplitControl>& split_control) {

    split_controls_.erase(split_control);

    if (split_control->X() < max_split_control_x_) {
        return;
    }

    //Recalculate max split control x.
    float new_max_x = 0;
    for (const auto& each_pair : split_controls_) {
        new_max_x = (std::max)(new_max_x, each_pair.first->X());
    }

    if (new_max_x == max_split_control_x_) {
        return;
    }

    max_split_control_x_ = new_max_x;
    max_split_control_x_changed_event_.GetObserver().OnNext(max_split_control_x_);
}

}