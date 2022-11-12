#include <zaf/control/internal/list_control/list_control_fixed_item_height_strategy.h>

namespace zaf::internal {

void ListControlFixedItemHeightStrategy::Initialize(
    ListDataSource& data_source,
    ListControlDelegate& delegate) {

    __super::Initialize(data_source, delegate);

    item_height_ = delegate.EstimateItemHeight(0, Object::Dumb());
    item_spacing_ = delegate.GetItemSpacing();
}


std::pair<float, float> ListControlFixedItemHeightStrategy::GetItemPositionAndHeight(
    std::size_t index) {

    float height = item_height_;
    float position = index * height;

    return std::make_pair(position, height);
}


std::pair<std::size_t, std::size_t> ListControlFixedItemHeightStrategy::GetItemIndexAndCount(
    float begin_position,
    float end_position) {

    std::size_t index = 0;
    std::size_t count = 0;

    auto begin_index = static_cast<std::size_t>(begin_position / item_height_);
    if (begin_index < ItemCount()) {

        index = begin_index;

        auto end_index = static_cast<std::size_t>(std::ceil(end_position / item_height_));
        if (end_index == begin_index) {
            count = 1;
        }
        else if (end_index >= ItemCount()) {
            count = ItemCount() - begin_index;
        }
        else {
            count = end_index - begin_index;
        }
    }

    return std::make_pair(index, count);
}


float ListControlFixedItemHeightStrategy::GetTotalHeight() {
    return ItemCount() * item_height_;
}

}