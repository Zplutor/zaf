#pragma once

#include <zaf/internal/list/list_item_height_strategy.h>

namespace zaf::internal {

class ListVariableItemHeightStrategy : public ListItemHeightStrategy {
public:
    using ListItemHeightStrategy::ListItemHeightStrategy;

    void Initialize() override;

    std::pair<float, float> GetItemPositionAndHeight(std::size_t index) override;

    std::optional<std::size_t> GetItemIndex(float position) override;

    std::pair<std::size_t, std::size_t> GetItemRange(
        float begin_position,
        float end_position) override;

    float GetTotalHeight() override;

    void OnDataAdded(const ListDataAddedInfo& event_info) override;
    void OnDataUpdated(const ListDataUpdatedInfo& event_info) override;
    void OnDataMoved(const ListDataMovedInfo& event_info) override;
    void OnDataRemoved(const ListDataRemovedInfo& event_info) override;

private:
    std::pair<float, float> InnerGetItemPositionAndHeight(std::size_t index) const;
    std::optional<std::size_t> InnerGetItemIndex(float position, bool skip_spacing) const;
    void UpdateItemHeightsInRange(const Range& range);

private:
    std::vector<float> item_positions_;
    float item_spacing_{};
};

}