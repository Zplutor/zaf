#include <zaf/control/internal/list_control/list_control_variable_item_height_strategy.h>

namespace zaf::internal {

void ListControlVariableItemHeightStrategy::Initialize(
    ListDataSource& data_source,
    ListControlDelegate& delegate) {

    __super::Initialize(data_source, delegate);

    item_positions_.reserve(ItemCount() + 1);

    float position = 0;

    for (std::size_t index = 0; index < ItemCount(); ++index) {

        item_positions_.push_back(position);

        auto item_data = data_source.GetDataAtIndex(index);
        position += delegate.EstimateItemHeight(index, item_data);
    }

    item_positions_.push_back(position);
}


std::pair<float, float> ListControlVariableItemHeightStrategy::GetItemPositionAndHeight(
    std::size_t index) {

    float position = 0;
    float height = 0;

    position = item_positions_[index];
    height = item_positions_[index + 1] - position;

    return std::make_pair(position, height);
}


std::pair<std::size_t, std::size_t> ListControlVariableItemHeightStrategy::GetItemIndexAndCount(
    float begin_position,
    float end_position) {

    std::size_t index = 0;
    std::size_t count = 0;

    //It is meaningless if the end_position is less than begin_position.
    if (begin_position <= end_position) {

        auto begin_iterator = std::upper_bound(item_positions_.begin(), item_positions_.end(), begin_position);
        if ((begin_iterator != item_positions_.begin()) && (begin_iterator != item_positions_.end())) {

            std::advance(begin_iterator, -1);
            index = std::distance(item_positions_.begin(), begin_iterator);

            auto end_iterator = std::lower_bound(item_positions_.begin(), item_positions_.end(), end_position);
            if (end_iterator != item_positions_.end()) {

                if (begin_iterator == end_iterator) {
                    count = 1;
                }
                else {
                    count = std::distance(begin_iterator, end_iterator);
                }
            }
            else {
                count = ItemCount() - index;
            }
        }
    }

    return std::make_pair(index, count);
}


float ListControlVariableItemHeightStrategy::GetTotalHeight() {

    if (!item_positions_.empty()) {
        return item_positions_.back();
    }
    else {
        return 0;
    }
}


void ListControlVariableItemHeightStrategy::OnItemAdd(
    const ListDataSourceDataAddInfo& event_info,
    ListDataSource& data_source,
    ListControlDelegate& delegate) {

    __super::OnItemAdd(event_info, data_source, delegate);

    item_positions_.insert(
        std::next(item_positions_.begin(), event_info.index),
        event_info.count,
        0);

    auto old_item_index = event_info.index + event_info.count;
    float old_position = item_positions_[old_item_index];
    float current_position = old_position;

    //Set position for new items.
    for (std::size_t current_index = event_info.index;
        current_index < old_item_index;
        ++current_index) {

        item_positions_[current_index] = current_position;

        auto item_data = data_source.GetDataAtIndex(current_index);
        current_position += delegate.EstimateItemHeight(current_index, item_data);
    }

    //Update position for old items.
    float position_increasement = current_position - old_position;
    for (std::size_t current_index = old_item_index; current_index < item_positions_.size(); ++current_index) {
        item_positions_[current_index] += position_increasement;
    }
}


void ListControlVariableItemHeightStrategy::OnItemUpdate(
    const ListDataSourceDataUpdateInfo& event_info,
    ListDataSource& data_source,
    ListControlDelegate& delegate) {

    __super::OnItemUpdate(event_info, data_source, delegate);

    float position = item_positions_[event_info.index];
    float previous_heights = item_positions_[event_info.index + event_info.count] - position;
    float current_heights = 0;

    for (std::size_t current_index = event_info.index;
        current_index < event_info.index + event_info.count;
        ++current_index) {

        item_positions_[current_index] = position + current_heights;

        auto item_data = data_source.GetDataAtIndex(current_index);
        float height = delegate.EstimateItemHeight(current_index, item_data);
        current_heights += height;
    }

    float difference = current_heights - previous_heights;

    for (std::size_t current_index = event_info.index + event_info.count;
        current_index < item_positions_.size();
        ++current_index) {

        item_positions_[current_index] += difference;
    }
}


void ListControlVariableItemHeightStrategy::OnItemRemove(
    const ListDataSourceDataRemoveInfo& event_info) {

    __super::OnItemRemove(event_info);

    float position_decreasement =
        item_positions_[event_info.index + event_info.count] - item_positions_[event_info.index];

    auto erase_iterator = std::next(item_positions_.begin(), event_info.index);
    auto remain_iterator = item_positions_.erase(
        erase_iterator,
        std::next(erase_iterator, event_info.count));

    for (; remain_iterator != item_positions_.end(); ++remain_iterator) {
        *remain_iterator -= position_decreasement;
    }
}

}