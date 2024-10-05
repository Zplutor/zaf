#pragma once

#include <memory>
#include <optional>
#include <zaf/base/non_copyable.h>
#include <zaf/control/internal/list_control/list_control_item_height_strategy.h>
#include <zaf/control/list_control_delegate.h>
#include <zaf/control/list_data_source.h>
#include <zaf/rx/subscription_set.h>

namespace zaf::internal {

class ListControlItemHeightManager : NonCopyableNonMovable {
public:
    ListControlItemHeightManager(const std::weak_ptr<ListDataSource>& data_source);
    ~ListControlItemHeightManager();

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
    void UnregisterDataSourceEvents();

    void ItemAdd(const ListDataAddedInfo& event_info);
    void ItemRemove(const ListDataRemovedInfo& event_info);
    void ItemUpdate(const ListDataUpdatedInfo& event_info);

private:
    std::weak_ptr<ListDataSource> data_source_{};
    std::weak_ptr<ListControlDelegate> delegate_{};
    SubscriptionSet data_source_subscriptions_;
    std::unique_ptr<ListControlItemHeightStrategy> strategy_;
};

}