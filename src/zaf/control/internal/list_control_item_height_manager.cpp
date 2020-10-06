#include <zaf/control/internal/list_control_item_height_manager.h>
#include <algorithm>
#include <zaf/base/assert.h>
#include <zaf/control/list_control_delegate.h>
#include <zaf/control/list_data_source.h>

namespace zaf {
namespace internal {

ListControlItemHeightManager::ListControlItemHeightManager(
    const std::shared_ptr<ListDataSource>& item_source,
    const std::shared_ptr<ListControlDelegate>& delegate)
    :
    data_source_(item_source),
    delegate_(delegate) {

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

    auto tag = reinterpret_cast<std::uintptr_t>(this);

    data_source->GetDataAddEvent().AddListenerWithTag(
        tag,
        std::bind(
            &ListControlItemHeightManager::ItemAdd, 
            this, 
            std::placeholders::_1, 
            std::placeholders::_2));

    data_source->GetDataRemoveEvent().AddListenerWithTag(
        tag,
        std::bind(
            &ListControlItemHeightManager::ItemRemove, 
            this,
            std::placeholders::_1,
            std::placeholders::_2));

    data_source->GetDataUpdateEvent().AddListenerWithTag(
        tag,
        std::bind(
            &ListControlItemHeightManager::ItemUpdate, 
            this, 
            std::placeholders::_1, 
            std::placeholders::_2));
}


void ListControlItemHeightManager::UnregisterDataSourceEvents() {

    auto data_source = data_source_.lock();
    if (!data_source) {
        return;
    }

    auto tag = reinterpret_cast<std::uintptr_t>(this);
    data_source->GetDataAddEvent().RemoveListenersWithTag(tag);
    data_source->GetDataRemoveEvent().RemoveListenersWithTag(tag);
    data_source->GetDataUpdateEvent().RemoveListenersWithTag(tag);
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

        item_height_ = delegate->EstimateItemHeight(0, Object::Empty());
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


void ListControlItemHeightManager::ItemAdd(std::size_t index, std::size_t count) {

    auto data_source = data_source_.lock();
    if (!data_source) {
        return;
    }

    auto delegate = delegate_.lock();
    if (!delegate) {
        return;
    }

    if (index > item_count_) {
        ZAF_FAIL();
        return;
    }

    item_count_ += count;

    if (! has_variable_heights_) {
        return;
    }

    item_positions_.insert(std::next(item_positions_.begin(), index), count, 0);

    auto old_item_index = index + count;
    float old_position = item_positions_[old_item_index];
    float current_position = old_position;

    //Set position for new items.
    for (std::size_t current_index = index; current_index < old_item_index; ++current_index) {

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


void ListControlItemHeightManager::ItemRemove(std::size_t index, std::size_t count) {

    if (index >= item_count_) {
        ZAF_FAIL();
        return;
    }

    if (count > item_count_ - index) {
        ZAF_FAIL();
        return;
    }

    item_count_ -= count;

    if (! has_variable_heights_) {
        return;
    }

    float position_decreasement = item_positions_[index + count] - item_positions_[index];

    auto erase_iterator = std::next(item_positions_.begin(), index);
    auto remain_iterator = item_positions_.erase(erase_iterator, std::next(erase_iterator, count));

    for (; remain_iterator != item_positions_.end(); ++remain_iterator) {
        *remain_iterator -= position_decreasement;
    }
}


void ListControlItemHeightManager::ItemUpdate(std::size_t index, std::size_t count) {

    auto data_source = data_source_.lock();
    if (!data_source) {
        return;
    }

    auto delegate = delegate_.lock();
    if (!delegate) {
        return;
    }

    if (index >= item_count_) {
        ZAF_FAIL();
        return;
    }

    if (count > item_count_ - index) {
        ZAF_FAIL();
        return;
    }

    if (! has_variable_heights_) {
        return;
    }

    float position = item_positions_[index];
    float previous_heights = item_positions_[index + count] - position;
    float current_heights = 0;

    for (std::size_t current_index = index; current_index < index + count; ++current_index) {

        item_positions_[current_index] = position + current_heights;

        auto item_data = data_source->GetDataAtIndex(current_index);
        float height = delegate->EstimateItemHeight(current_index, item_data);
        current_heights += height;
    }

    float difference = current_heights - previous_heights;

    for (std::size_t current_index = index + count; current_index < item_positions_.size(); ++current_index) {
        item_positions_[current_index] += difference;
    }
}

}
}