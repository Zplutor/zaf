#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/control/list_control_delegate.h>
#include <zaf/control/list_data_source.h>

namespace zaf::internal {

class ListControlItemHeightStrategy : NonCopyableNonMovable {
public:
    virtual ~ListControlItemHeightStrategy() = default;

    virtual void Initialize(
        ListDataSource& data_source,
        ListControlDelegate& delegate);

    virtual std::pair<float, float> GetItemPositionAndHeight(std::size_t index) = 0;

    virtual std::optional<std::size_t> GetItemIndex(float position) = 0;

    virtual std::pair<std::size_t, std::size_t> GetItemRange(
        float begin_position,
        float end_position) = 0;

    virtual float GetTotalHeight() = 0;

    virtual void OnItemAdd(
        const ListDataSourceDataAddInfo& event_info,
        ListDataSource& data_source,
        ListControlDelegate& delegate);

    virtual void OnItemUpdate(
        const ListDataSourceDataUpdateInfo& event_info,
        ListDataSource& data_source,
        ListControlDelegate& delegate);

    virtual void OnItemRemove(const ListDataSourceDataRemoveInfo& event_info);

    std::size_t ItemCount() const {
        return item_count_;
    }

private:
    std::size_t item_count_{};
};

}