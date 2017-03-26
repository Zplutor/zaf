#include <zaf/control/internal/list_control_item_height_manager.h>
#include <algorithm>
#include <zaf/base/assert.h>

namespace zaf {
namespace internal {

ListControlItemHeightManager::ListControlItemHeightManager(const std::shared_ptr<ListControl::ItemSource>& item_source) :
    item_source_(item_source),
    item_count_(0),
    has_variable_heights_(false),
    item_height_(0) {

    auto tag = reinterpret_cast<std::uintptr_t>(this);

    item_source_->GetItemAddEvent().AddListenerWithTag(
        tag,
        std::bind(&ListControlItemHeightManager::ItemAdd, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    item_source_->GetItemRemoveEvent().AddListenerWithTag(
        tag,
        std::bind(&ListControlItemHeightManager::ItemRemove, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    item_source_->GetItemUpdateEvent().AddListenerWithTag(
        tag,
        std::bind(&ListControlItemHeightManager::ItemUpdate, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}


ListControlItemHeightManager::~ListControlItemHeightManager() {

    auto tag = reinterpret_cast<std::uintptr_t>(this);
    item_source_->GetItemAddEvent().RemoveListenersWithTag(tag);
    item_source_->GetItemRemoveEvent().RemoveListenersWithTag(tag);
    item_source_->GetItemUpdateEvent().RemoveListenersWithTag(tag);
}


void ListControlItemHeightManager::LoadItemHeights() {

    item_count_ = item_source_->GetItemCount();
    has_variable_heights_ = item_source_->HasVariableItemHeight();

    item_positions_.clear();

    if (has_variable_heights_) {

        item_positions_.reserve(item_count_ + 1);

        float position = 0;

        for (std::size_t index = 0; index < item_count_; ++index) {

            item_positions_.push_back(position);
            position += item_source_->GetItemHeight(index);
        }

        item_positions_.push_back(position);
    }
    else {

        item_height_ = item_source_->GetItemHeight(0);
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


void ListControlItemHeightManager::ItemAdd(
    ListControl::ItemSource& item_source,
    std::size_t index, 
    std::size_t count) {

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
        current_position += item_source.GetItemHeight(current_index);
    }

    //Update position for old items.
    float position_increasement = current_position - old_position;
    for (std::size_t current_index = old_item_index; current_index < item_positions_.size(); ++current_index) {
        item_positions_[current_index] += position_increasement;
    }
}


void ListControlItemHeightManager::ItemRemove(
    ListControl::ItemSource& item_source,
    std::size_t index,
    std::size_t count) {

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


void ListControlItemHeightManager::ItemUpdate(
    ListControl::ItemSource& item_source,
    std::size_t index,
    std::size_t count) {

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

        float height = item_source.GetItemHeight(current_index);
        current_heights += height;
    }

    float difference = current_heights - previous_heights;

    for (std::size_t current_index = index + count; current_index < item_positions_.size(); ++current_index) {
        item_positions_[current_index] += difference;
    }
}

}
}