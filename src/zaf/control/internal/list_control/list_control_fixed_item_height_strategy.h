#pragma once

#include <zaf/control/internal/list_control/list_control_item_height_strategy.h>

namespace zaf::internal {

class ListControlFixedItemHeightStrategy : public ListControlItemHeightStrategy {
public:
    void Initialize(
        ListDataSource& data_source,
        ListControlDelegate& delegate) override;

    std::pair<float, float> GetItemPositionAndHeight(std::size_t index) override;

    std::pair<std::size_t, std::size_t> GetItemIndexAndCount(
        float begin_position,
        float end_position) override;

    float GetTotalHeight() override;

private:
    float item_height_{};
    float item_spacing_{};
};

}