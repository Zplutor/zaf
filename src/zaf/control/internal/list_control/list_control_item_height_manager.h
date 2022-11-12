#pragma once

#include <memory>
#include <zaf/base/non_copyable.h>
#include <zaf/control/internal/list_control/list_control_item_height_strategy.h>
#include <zaf/control/list_control_delegate.h>
#include <zaf/control/list_data_source.h>
#include <zaf/rx/subscription_holder.h>

namespace zaf::internal {

class ListControlItemHeightManager : NonCopyable {
public:
    ListControlItemHeightManager(const std::shared_ptr<ListDataSource>& data_source);
    ~ListControlItemHeightManager();

    void ResetDelegate(const std::shared_ptr<ListControlDelegate>& delegate);

    void ReloadItemHeights();

    std::pair<float, float> GetItemPositionAndHeight(std::size_t index) const;
    std::pair<std::size_t, std::size_t> GetItemIndexAndCount(
        float begin_position, 
        float end_position) const;

    float GetTotalHeight() const;

    std::size_t GetItemCount() const {
        return strategy_->ItemCount();
    }

private:
    void RegisterDataSourceEvents();
    void UnregisterDataSourceEvents();

    void ItemAdd(const ListDataSourceDataAddInfo& event_info);
    void ItemRemove(const ListDataSourceDataRemoveInfo& event_info);
    void ItemUpdate(const ListDataSourceDataUpdateInfo& event_info);

private:
    std::weak_ptr<ListDataSource> data_source_{};
    std::weak_ptr<ListControlDelegate> delegate_{};
    SubscriptionHolder data_source_subscriptions_;
    std::unique_ptr<ListControlItemHeightStrategy> strategy_;
};

}