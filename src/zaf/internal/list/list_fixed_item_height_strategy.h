#pragma once

#include <zaf/internal/list/list_item_height_strategy.h>

namespace zaf::internal {

class ListFixedItemHeightStrategy : public ListItemHeightStrategy {
public:
    using ListItemHeightStrategy::ListItemHeightStrategy;

    void Initialize() override;

    std::pair<float, float> GetItemPositionAndHeight(std::size_t index) override;

    std::optional<std::size_t> GetItemIndex(float position) override;

    std::pair<std::size_t, std::size_t> GetItemRange(
        float begin_position,
        float end_position) override;

    float GetTotalHeight() override;

private:
    float GetItemPosition(std::size_t index) const;
    std::optional<std::size_t> InnerGetItemIndex(float position, bool skip_spacing) const;

private:
    float item_height_{};
    float item_spacing_{};
};

}