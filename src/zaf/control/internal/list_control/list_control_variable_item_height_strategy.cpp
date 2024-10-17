#include <zaf/control/internal/list_control/list_control_variable_item_height_strategy.h>

namespace zaf::internal {

void ListVariableItemHeightStrategy::Initialize(
    ListDataSource& data_source,
    ListControlDelegate& delegate) {

    __super::Initialize(data_source, delegate);

    item_spacing_ = delegate.GetItemSpacing();
    item_positions_.reserve(ItemCount() + 1);

    float position = 0;
    for (std::size_t index = 0; index < ItemCount(); ++index) {

        item_positions_.push_back(position);

        auto item_data = data_source.GetDataAtIndex(index);
        position += delegate.EstimateItemHeight(index, item_data) + item_spacing_;
    }

    item_positions_.push_back(position);
}


std::pair<float, float> ListVariableItemHeightStrategy::GetItemPositionAndHeight(
    std::size_t index) {

    return InnerGetItemPositionAndHeight(index);
}


std::pair<float, float> ListVariableItemHeightStrategy::InnerGetItemPositionAndHeight(
    std::size_t index) const {

    ZAF_EXPECT(index < item_positions_.size() - 1);

    float position = item_positions_[index];
    float height = item_positions_[index + 1] - position - item_spacing_;

    return std::make_pair(position, height);
}


std::optional<std::size_t> ListVariableItemHeightStrategy::GetItemIndex(float position) {
    return InnerGetItemIndex(position, false);
}


std::optional<std::size_t> ListVariableItemHeightStrategy::InnerGetItemIndex(
    float position,
    bool skip_spacing) const {

    auto iterator = std::upper_bound(item_positions_.begin(), item_positions_.end(), position);
    if (iterator == item_positions_.begin() ||
        iterator == item_positions_.end()) {
        return std::nullopt;
    }

    std::advance(iterator, -1);
    std::size_t index = std::distance(item_positions_.begin(), iterator);

    auto item_position_height = InnerGetItemPositionAndHeight(index);
    if (item_position_height.first + item_position_height.second <= position) {
        //Psition is in spacing.
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


std::pair<std::size_t, std::size_t> ListVariableItemHeightStrategy::GetItemRange(
    float begin_position,
    float end_position) {

    ZAF_EXPECT(begin_position < end_position);

    auto begin_index = InnerGetItemIndex(begin_position, true);
    if (!begin_index) {
        return {};
    }

    auto end_iterator = std::lower_bound(
        item_positions_.begin(), 
        item_positions_.end(),
        end_position);

    std::size_t end_index{};
    if (end_iterator != item_positions_.end()) {
        end_index = std::distance(item_positions_.begin(), end_iterator);
    }
    else {
        end_index = ItemCount();
    }

    return std::make_pair(*begin_index, end_index - *begin_index);
}


float ListVariableItemHeightStrategy::GetTotalHeight() {

    if (item_positions_.size() > 1) {
        return item_positions_.back() - item_spacing_;
    }
    else {
        return 0;
    }
}


void ListVariableItemHeightStrategy::OnDataAdded(
    const ListDataAddedInfo& event_info,
    ListDataSource& data_source,
    ListControlDelegate& delegate) {

    __super::OnDataAdded(event_info, data_source, delegate);

    item_positions_.insert(
        std::next(item_positions_.begin(), event_info.Index()),
        event_info.Count(),
        0);

    auto old_item_index = event_info.Index() + event_info.Count();
    float old_position = item_positions_[old_item_index];
    float current_position = old_position;

    //Set position for new items.
    for (std::size_t current_index = event_info.Index();
        current_index < old_item_index;
        ++current_index) {

        item_positions_[current_index] = current_position;

        auto item_data = data_source.GetDataAtIndex(current_index);
        current_position += delegate.EstimateItemHeight(current_index, item_data) + item_spacing_;
    }

    //Update position for old items.
    float position_increasement = current_position - old_position;
    for (std::size_t current_index = old_item_index; current_index < item_positions_.size(); ++current_index) {
        item_positions_[current_index] += position_increasement;
    }
}


void ListVariableItemHeightStrategy::OnDataUpdated(
    const ListDataUpdatedInfo& event_info,
    ListDataSource& data_source,
    ListControlDelegate& delegate) {

    __super::OnDataUpdated(event_info, data_source, delegate);

    UpdateItemHeightsInRange(
        Range{ event_info.Index(), event_info.Count() },
        data_source,
        delegate);
}


void ListVariableItemHeightStrategy::OnDataMoved(
    const ListDataMovedInfo& event_info,
    ListDataSource& data_source,
    ListControlDelegate& delegate) {

    __super::OnDataMoved(event_info, data_source, delegate);

    auto min_index = std::min(event_info.PreviousIndex(), event_info.NewIndex());
    auto max_index = std::max(event_info.PreviousIndex(), event_info.NewIndex());

    UpdateItemHeightsInRange(
        Range::FromIndexPair(min_index, max_index + 1), 
        data_source, 
        delegate);
}


void ListVariableItemHeightStrategy::UpdateItemHeightsInRange(
    const Range& range, 
    ListDataSource& data_source, 
    ListControlDelegate& delegate) {

    float position = item_positions_[range.Index()];
    float previous_heights = item_positions_[range.EndIndex()] - position;
    float current_heights = 0;

    for (std::size_t current_index = range.Index();
        current_index < range.EndIndex();
        ++current_index) {

        item_positions_[current_index] = position + current_heights;

        auto item_data = data_source.GetDataAtIndex(current_index);
        float height = delegate.EstimateItemHeight(current_index, item_data);
        current_heights += height + item_spacing_;
    }

    float difference = current_heights - previous_heights;

    for (std::size_t current_index = range.EndIndex();
        current_index < item_positions_.size();
        ++current_index) {

        item_positions_[current_index] += difference;
    }
}


void ListVariableItemHeightStrategy::OnDataRemoved(
    const ListDataRemovedInfo& event_info) {

    __super::OnDataRemoved(event_info);

    float position_decreasement =
        item_positions_[event_info.Index() + event_info.Count()] - item_positions_[event_info.Index()];

    auto erase_iterator = std::next(item_positions_.begin(), event_info.Index());
    auto remain_iterator = item_positions_.erase(
        erase_iterator,
        std::next(erase_iterator, event_info.Count()));

    for (; remain_iterator != item_positions_.end(); ++remain_iterator) {
        *remain_iterator -= position_decreasement;
    }
}

}