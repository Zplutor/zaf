#include <zaf/internal/list/list_fixed_item_height_strategy.h>

namespace zaf::internal {

void ListFixedItemHeightStrategy::Initialize() {

    __super::Initialize();

    auto delegate = Delegate();
    if (!delegate) {
        return;
    }

    item_height_ = delegate->EstimateItemHeight(0, nullptr);
    item_spacing_ = delegate->GetItemSpacing();
}


std::pair<float, float> ListFixedItemHeightStrategy::GetItemPositionAndHeight(std::size_t index) {

    auto data_source = DataSource();
    if (!data_source) {
        return {};
    }

    if (index >= data_source->GetDataCount()) {
        return {};
    }

    float position = GetItemPosition(index);
    return std::make_pair(position, item_height_);
}


float ListFixedItemHeightStrategy::GetItemPosition(std::size_t index) const {
    return index * item_height_ + index * item_spacing_;
}


std::optional<std::size_t> ListFixedItemHeightStrategy::GetItemIndex(float position) {
    return InnerGetItemIndex(position, false);
}


std::optional<std::size_t> ListFixedItemHeightStrategy::InnerGetItemIndex(
    float position, 
    bool skip_spacing) const {

    auto data_source = DataSource();
    if (!data_source) {
        return std::nullopt;
    }

    auto index = static_cast<std::size_t>(position / (item_height_ + item_spacing_));

    if (GetItemPosition(index) + item_height_ <= position) {
        //Position is in spacing.
        if (!skip_spacing) {
            return std::nullopt;
        }
        index++;
    }

    if (index >= data_source->GetDataCount()) {
        return std::nullopt;
    }
    return index;
}


std::pair<std::size_t, std::size_t> ListFixedItemHeightStrategy::GetItemRange(
    float begin_position,
    float end_position) {

    ZAF_EXPECT(begin_position < end_position);

    auto data_source = DataSource();
    if (!data_source) {
        return {};
    }

    //Find index of the first item.
    auto begin_index = InnerGetItemIndex(begin_position, true);
    if (!begin_index) {
        return {};
    }

    //Find index of the end item (note that it is not the last item).
    auto end_index = 
        static_cast<std::size_t>(std::ceil(end_position / (item_height_ + item_spacing_)));

    auto data_count = data_source->GetDataCount();
    if (end_index > data_count) {
        end_index = data_count;
    }

    return std::make_pair(*begin_index, end_index - *begin_index);
}


float ListFixedItemHeightStrategy::GetTotalHeight() {

    auto data_source = DataSource();
    if (!data_source) {
        return 0;
    }

    auto data_count = data_source->GetDataCount();
    if (data_count == 0) {
        return 0;
    }

    return 
        data_count * item_height_ +
        (data_count - 1) * item_spacing_;
}

}