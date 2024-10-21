#pragma once

#include <zaf/control/list_item.h>
#include <zaf/internal/list/list_control_part.h>

namespace zaf::internal {

class ListVisibleItemManager : public ListControlPart {
public:
    using ListControlPart::ListControlPart;

    bool HasVisibleItem() const noexcept;
    std::shared_ptr<ListItem> GetVisibleItemAtIndex(std::size_t index) const noexcept;
    std::optional<std::size_t> GetIndexOfVisibleItem(const ListItem& item) const noexcept;

    void HandleDataAdded(const Range& added_range, float height_difference);
    void HandleDataRemoved(const Range& removed_range, float height_difference);
    void HandleDataUpdated(const Range& updated_range, float height_difference);
    void HandleDataMoved(std::size_t previous_index, std::size_t new_index);

    void UpdateVisibleItems();

private:
    std::optional<std::size_t> AddVisibleItems(const Range& added_range);
    std::optional<std::size_t> AddMiddleVisibleItems(const Range& added_range);

    std::optional<std::size_t> RemoveVisibleItems(const Range& removed_range);
    std::size_t RemoveMiddleVisibleItems(const Range& removed_range);

    void AdjustVisibleItemPositionsByUpdatingItems(
        const Range& updated_range,
        float position_difference);
    void UpdateVisibleItemsByUpdatingItems(const Range& updated_range);

    void AdjustVisibleItemPositions(std::size_t begin_adjust_index, float difference);

    void GetVisibleItemsRange(std::size_t& index, std::size_t& count);
    void AdjustVisibleItems(
        std::size_t new_index,
        std::size_t new_count,
        bool remove_head,
        std::size_t head_change_count,
        bool remove_tail,
        std::size_t tail_change_count);
    void RemoveHeadVisibleItems(std::size_t count);
    void RemoveTailVisibleItems(std::size_t count);
    std::vector<std::shared_ptr<ListItem>> CreateItems(std::size_t index, std::size_t count);
    std::shared_ptr<ListItem> CreateItem(std::size_t index);

private:
    std::size_t first_visible_item_index_{};
    std::deque<std::shared_ptr<ListItem>> visible_items_;
};

}
