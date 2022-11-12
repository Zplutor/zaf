#pragma once

#include <zaf/control/internal/list_control/list_control_item_height_strategy.h>

namespace zaf::internal {

class ListControlVariableItemHeightStrategy : public ListControlItemHeightStrategy {
public:
    void Initialize(
        ListDataSource& data_source,
        ListControlDelegate& delegate) override;

    std::pair<float, float> GetItemPositionAndHeight(std::size_t index) override;

    std::pair<std::size_t, std::size_t> GetItemIndexAndCount(
        float begin_position,
        float end_position) override;

    float GetTotalHeight() override;

    void OnItemAdd(
        const ListDataSourceDataAddInfo& event_info,
        ListDataSource& data_source,
        ListControlDelegate& delegate) override;

    void OnItemUpdate(
        const ListDataSourceDataUpdateInfo& event_info,
        ListDataSource& data_source,
        ListControlDelegate& delegate) override;

    void OnItemRemove(const ListDataSourceDataRemoveInfo& event_info) override;

private:
    std::vector<float> item_positions_;
};

}