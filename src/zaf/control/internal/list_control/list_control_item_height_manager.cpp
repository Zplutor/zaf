#include <zaf/control/internal/list_control/list_control_item_height_manager.h>
#include <algorithm>

namespace zaf::internal {

ListControlItemHeightManager::ListControlItemHeightManager(
    const std::shared_ptr<ListDataSource>& item_source) : data_source_(item_source) {

    RegisterDataSourceEvents();
}


ListControlItemHeightManager::~ListControlItemHeightManager() {
    UnregisterDataSourceEvents();
}


void ListControlItemHeightManager::RegisterDataSourceEvents() {

    auto data_source = data_source_.lock();
    if (!data_source) {
        return;
    }

    data_source_subscriptions_ += data_source->DataAddEvent().Subscribe(
        std::bind(
            &ListControlItemHeightManager::ItemAdd, 
            this, 
            std::placeholders::_1));

    data_source_subscriptions_ += data_source->DataRemoveEvent().Subscribe(
        std::bind(
            &ListControlItemHeightManager::ItemRemove, 
            this,
            std::placeholders::_1));

    data_source_subscriptions_ += data_source->DataUpdateEvent().Subscribe(
        std::bind(
            &ListControlItemHeightManager::ItemUpdate, 
            this, 
            std::placeholders::_1));
}


void ListControlItemHeightManager::UnregisterDataSourceEvents() {

    data_source_subscriptions_.Clear();
}


void ListControlItemHeightManager::ResetDelegate(
    const std::shared_ptr<ListControlDelegate>& delegate) {

    delegate_ = delegate;
    ReloadItemHeights();
}


void ListControlItemHeightManager::ReloadItemHeights() {

    auto data_source = data_source_.lock();
    if (!data_source) {
        return;
    }

    auto delegate = delegate_.lock();
    if (!delegate) {
        return;
    }

    item_count_ = data_source->GetDataCount();
    has_variable_heights_ = delegate->HasVariableItemHeight();

    item_positions_.clear();

    if (has_variable_heights_) {

        item_positions_.reserve(item_count_ + 1);

        float position = 0;

        for (std::size_t index = 0; index < item_count_; ++index) {

            item_positions_.push_back(position);

            auto item_data = data_source->GetDataAtIndex(index);
            position += delegate->EstimateItemHeight(index, item_data);
        }

        item_positions_.push_back(position);
    }
    else {

        item_height_ = delegate->EstimateItemHeight(0, Object::Dumb());
    }
}


std::pair<float, float> ListControlItemHeightManager::GetItemPositionAndHeight(std::size_t index) const {

    float position = 0;
    float height = 0;

    if (index < item_count_) {
        if (has_variable_heights_) {
            position = item_positions_[index];
            height = item_positions_[index + 1] - position;
        }
        else {
            height = item_height_;
            position = index * height;
        }
    }
    
    return std::make_pair(position, height);
}


std::pair<std::size_t, std::size_t> ListControlItemHeightManager::GetItemIndexAndCount(float begin_position, float end_position) const {

    std::size_t index = 0;
    std::size_t count = 0;

    //Is is meaningless if the end_position is less than begin_position.
    if (begin_position <= end_position) {
        if (has_variable_heights_) {

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
                    count = item_count_ - index;
                }
            }
        }
        else {

            auto begin_index = static_cast<std::size_t>(begin_position / item_height_);
            if (begin_index < item_count_) {

                index = begin_index;

                auto end_index = static_cast<std::size_t>(std::ceil(end_position / item_height_));
                if (end_index == begin_index) {
                    count = 1;
                }
                else if (end_index >= item_count_) {
                    count = item_count_ - begin_index;
                }
                else {
                    count = end_index - begin_index;
                }
            }
        }
    }

    return std::make_pair(index, count);
}


float ListControlItemHeightManager::GetTotalHeight() const {

    if (has_variable_heights_) {

        if (! item_positions_.empty()) {
            return item_positions_.back();
        }
        else {
            return 0;
        }
    }
    else {
        return item_count_ * item_height_;
    }
}


void ListControlItemHeightManager::ItemAdd(const ListDataSourceDataAddInfo& event_info) {

    auto data_source = data_source_.lock();
    if (!data_source) {
        return;
    }

    auto delegate = delegate_.lock();
    if (!delegate) {
        return;
    }

    if (event_info.index > item_count_) {
        ZAF_ALERT();
        return;
    }

    item_count_ += event_info.count;

    if (! has_variable_heights_) {
        return;
    }

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

        auto item_data = data_source->GetDataAtIndex(current_index);
        current_position += delegate->EstimateItemHeight(current_index, item_data);
    }

    //Update position for old items.
    float position_increasement = current_position - old_position;
    for (std::size_t current_index = old_item_index; current_index < item_positions_.size(); ++current_index) {
        item_positions_[current_index] += position_increasement;
    }
}


void ListControlItemHeightManager::ItemRemove(const ListDataSourceDataRemoveInfo& event_info) {

    if (event_info.index >= item_count_) {
        ZAF_ALERT();
        return;
    }

    if (event_info.count > item_count_ - event_info.index) {
        ZAF_ALERT();
        return;
    }

    item_count_ -= event_info.count;

    if (! has_variable_heights_) {
        return;
    }

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


void ListControlItemHeightManager::ItemUpdate(const ListDataSourceDataUpdateInfo& event_info) {

    auto data_source = data_source_.lock();
    if (!data_source) {
        return;
    }

    auto delegate = delegate_.lock();
    if (!delegate) {
        return;
    }

    if (event_info.index >= item_count_) {
        ZAF_ALERT();
        return;
    }

    if (event_info.count > item_count_ - event_info.index) {
        ZAF_ALERT();
        return;
    }

    if (! has_variable_heights_) {
        return;
    }

    float position = item_positions_[event_info.index];
    float previous_heights = item_positions_[event_info.index + event_info.count] - position;
    float current_heights = 0;

    for (std::size_t current_index = event_info.index; 
         current_index < event_info.index + event_info.count; 
         ++current_index) {

        item_positions_[current_index] = position + current_heights;

        auto item_data = data_source->GetDataAtIndex(current_index);
        float height = delegate->EstimateItemHeight(current_index, item_data);
        current_heights += height;
    }

    float difference = current_heights - previous_heights;

    for (std::size_t current_index = event_info.index + event_info.count; 
         current_index < item_positions_.size(); 
         ++current_index) {

        item_positions_[current_index] += difference;
    }
}

}