#pragma once

#include <memory>
#include <optional>
#include <zaf/base/non_copyable.h>
#include <zaf/internal/list/list_item_height_strategy.h>
#include <zaf/control/list_control_delegate.h>
#include <zaf/control/list_data_source.h>
#include <zaf/rx/subscription_set.h>

namespace zaf::internal {

class ListItemHeightManager : NonCopyableNonMovable {
public:
    ListItemHeightManager() = default;

    void ResetDataSource(std::weak_ptr<ListDataSource> data_source);
    void ResetDelegate(const std::weak_ptr<ListControlDelegate>& delegate);

    void ReloadItemHeights();

    std::pair<float, float> GetItemPositionAndHeight(std::size_t index) const;

    std::optional<std::size_t> GetItemIndex(float position) const;

    //Note that the range is [begin_position, end_position)
    std::pair<std::size_t, std::size_t> GetItemRange(
        float begin_position, 
        float end_position) const;

    float GetTotalHeight() const;

    std::size_t GetItemCount() const {
        if (strategy_) {
            return strategy_->ItemCount();
        }
        return 0;
    }

private:
    void RegisterDataSourceEvents();

    void OnDataAdded(const ListDataAddedInfo& event_info);
    void OnDataRemoved(const ListDataRemovedInfo& event_info);
    void OnDataUpdated(const ListDataUpdatedInfo& event_info);
    void OnDataMoved(const ListDataMovedInfo& event_info);

private:
    std::weak_ptr<ListDataSource> data_source_{};
    std::weak_ptr<ListControlDelegate> delegate_{};
    SubscriptionSet data_source_subs_;
    std::unique_ptr<ListItemHeightStrategy> strategy_;
};

}