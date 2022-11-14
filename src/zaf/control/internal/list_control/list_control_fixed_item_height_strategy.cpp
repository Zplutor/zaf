#include <zaf/control/internal/list_control/list_control_fixed_item_height_strategy.h>

namespace zaf::internal {

void ListControlFixedItemHeightStrategy::Initialize(
    ListDataSource& data_source,
    ListControlDelegate& delegate) {

    __super::Initialize(data_source, delegate);

    item_height_ = delegate.EstimateItemHeight(0, nullptr);
    item_spacing_ = delegate.GetItemSpacing();
}


std::pair<float, float> ListControlFixedItemHeightStrategy::GetItemPositionAndHeight(
    std::size_t index) {

    float position = GetItemPosition(index);
    return std::make_pair(position, item_height_);
}


float ListControlFixedItemHeightStrategy::GetItemPosition(std::size_t index) const {
    return index * item_height_ + index * item_spacing_;
}


std::optional<std::size_t> ListControlFixedItemHeightStrategy::GetItemIndex(float position) {
    return InnerGetItemIndex(position, false);
}


std::optional<std::size_t> ListControlFixedItemHeightStrategy::InnerGetItemIndex(
    float position, 
    bool skip_spacing) const {

    auto index = static_cast<std::size_t>(position / (item_height_ + item_spacing_));

    if (GetItemPosition(index) + item_height_ <= position) {
        //Position is in spacing.
        if (!skip_spacing) {
            return std::nullopt;
        }
        index++;
    }

    if (index >= ItemCount()) {
        return std::nullopt;
    }
    return index;
}


std::pair<std::size_t, std::size_t> ListControlFixedItemHeightStrategy::GetItemRange(
    float begin_position,
    float end_position) {

    ZAF_EXPECT(begin_position < end_position);

    //Find index of the first item.
    auto begin_index = InnerGetItemIndex(begin_position, true);
    if (!begin_index) {
        return {};
    }

    //Find index of the end item (note that it is not the last item).
    auto end_index = 
        static_cast<std::size_t>(std::ceil(end_position / (item_height_ + item_spacing_)));

    if (end_index > ItemCount()) {
        end_index = ItemCount();
    }

    return std::make_pair(*begin_index, end_index - *begin_index);
}


float ListControlFixedItemHeightStrategy::GetTotalHeight() {

    if (ItemCount() == 0) {
        return 0;
    }

    return 
        ItemCount() * item_height_ + 
        (ItemCount() - 1) * item_spacing_;
}



}