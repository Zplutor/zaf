#include <zaf/control/internal/list_control_item_selection_manager.h>
#include <algorithm>
#include <zaf/base/define.h>

namespace zaf {
namespace internal {

inline bool IsIndexInRange(std::size_t index, const std::pair<std::size_t, std::size_t>& range) {
    return (range.first <= index) && (index < range.first + range.second);
}

void ListControlItemSelectionManager::ReplaceSelection(std::size_t index, std::size_t count) {

    selections_.clear();

    if (count != 0) {
        AddSelectionRange(std::make_pair(index, count));
    }
}


void ListControlItemSelectionManager::RevertSelection(std::size_t index) {

    auto iterator = selections_.begin();
    while (iterator != selections_.end()) {

        if (! IsIndexInRange(index, *iterator)) {
            ++iterator;
            continue;
        }

        std::size_t new_range_index = index + 1;
        std::size_t new_range_count = iterator->first + iterator->second - new_range_index;

        std::size_t old_range_count = index - iterator->first;
        if (old_range_count == 0) {
            iterator = selections_.erase(iterator);
        }

        if (new_range_count != 0) {
            iterator = selections_.insert(iterator, std::make_pair(new_range_index, new_range_count));
        }
        return;
    }

    AddSelectionRange(std::make_pair(index, 1));
}


void ListControlItemSelectionManager::AddSelection(std::size_t index, std::size_t count) {
    
    std::size_t need_added_range_index = index;
    std::size_t need_added_range_end_index = index + count;

    auto iterator = selections_.begin();
    while (iterator != selections_.end()) {
        
        std::size_t current_range_index = iterator->first;
        std::size_t current_range_end_index = iterator->first + iterator->second;

        if (need_added_range_index < current_range_end_index) {

            if (need_added_range_end_index <= current_range_index) {

                std::size_t add_count = need_added_range_end_index - need_added_range_index;
                iterator = selections_.insert(iterator, std::make_pair(need_added_range_index, add_count));
            }
            else if (need_added_range_index < current_range_index) {

                std::size_t add_count = current_range_index - need_added_range_index;
                iterator = selections_.insert(iterator,std::make_pair(need_added_range_index, add_count));
            }

            need_added_range_index = current_range_end_index;

            if (need_added_range_index >= need_added_range_end_index) {
                return;
            }
        }

        ++iterator;
    }

    selections_.insert(
        iterator, 
        std::make_pair(need_added_range_index, need_added_range_end_index - need_added_range_index));
}


void ListControlItemSelectionManager::RemoveSelection(std::size_t index, std::size_t count) {

    std::size_t need_remove_range_index = index;
    std::size_t need_remove_range_end_index = index + count;

    auto iterator = selections_.begin();
    while (iterator != selections_.end()) {

        std::size_t current_range_index = iterator->first;
        std::size_t current_range_end_index = iterator->first + iterator->second;

        std::size_t intersect_index = std::max(current_range_index, need_remove_range_index);
        std::size_t intersect_end_index = std::min(current_range_end_index, need_remove_range_end_index);
        if (intersect_index >= intersect_end_index) {
            ++iterator;
            continue;
        }

        std::size_t new_range_index = intersect_end_index;
        std::size_t new_range_count = current_range_end_index - new_range_index;

        iterator->second -= intersect_end_index - intersect_index;

        if (iterator->second == 0) {

            if (new_range_count != 0) {
                iterator->first = new_range_index;
                iterator->second = new_range_count;
            }
            else {
                iterator = selections_.erase(iterator);
            }
        }
        else {

            if (new_range_count != 0) {
                iterator = selections_.insert(iterator, std::make_pair(new_range_index, new_range_count));
            }
            else {
                ++iterator;
            }
        }
    }
}


bool ListControlItemSelectionManager::AdjustSelectionByAddingIndexes(std::size_t add_index, std::size_t add_count) {

    bool selection_changed = false;

    auto iterator = selections_.begin();
    while (iterator != selections_.end()) {

        if (iterator->first >= add_index) {
            iterator->first += add_count;
            selection_changed = true;
        }
        else if (iterator->first + iterator->second > add_index) {

            std::size_t new_range_index = add_index + add_count;
            std::size_t new_range_count = iterator->second - add_index;

            iterator->second = add_index - iterator->first;
            if (iterator->second == 0) {
                iterator = selections_.erase(iterator);
            }

            iterator = selections_.insert(std::next(iterator), std::make_pair(new_range_index, new_range_count));
            selection_changed = true;
        }

        ++iterator;
    }

    return selection_changed;
}


bool ListControlItemSelectionManager::AdjustSelectionByRemovingIndexes(std::size_t remove_index, std::size_t remove_count) {

    bool selection_changed = false;

    std::size_t remove_end_index = remove_index + remove_count;

    auto iterator = selections_.begin();
    while (iterator != selections_.end()) {

        if (remove_end_index <= iterator->first) {
            iterator->first -= remove_count;
            selection_changed = true;
        }
        else {

            std::size_t intersect_begin_index = std::max(remove_index, iterator->first);
            std::size_t intersect_end_index = std::min(remove_end_index, iterator->first + iterator->second);

            if (intersect_end_index > intersect_begin_index) {

                std::size_t intersect_count = intersect_end_index - intersect_begin_index;
                iterator->second -= intersect_count;

                if (intersect_begin_index == iterator->first) {
                    iterator->first = iterator->first + intersect_count - remove_count;
                }

                selection_changed = true;
            }
        }

        ++iterator;
    }

    return selection_changed;
}


void ListControlItemSelectionManager::AddSelectionRange(const std::pair<std::size_t, std::size_t>& range) {

    auto insert_iterator = std::upper_bound(
        selections_.begin(),
        selections_.end(),
        range, 
        [](const std::pair<std::size_t, std::size_t>& range1, const std::pair<std::size_t, std::size_t>& range2) {
            return range1.first < range2.first;
        }
    );

    if (insert_iterator == selections_.end()) {
        selections_.push_back(range);
    }
    else {
        selections_.insert(insert_iterator, range);
    }
}


std::size_t ListControlItemSelectionManager::GetAllSelectedCount() const {

    std::size_t count = 0;

    for (const auto& each_selection : selections_) {
        count += each_selection.second;
    }

    return count;
}


std::vector<std::size_t> ListControlItemSelectionManager::GetAllSelectedIndexes() const {

    std::vector<std::size_t> indexes;

    for (const auto& each_selection : selections_) {

        for (std::size_t index = each_selection.first;
             index < each_selection.first + each_selection.second;
             ++index) {

            indexes.push_back(index);
        }
    }

    return indexes;
}


std::size_t ListControlItemSelectionManager::GetFirstSelectedIndex() const {

    if (selections_.empty()) {
        return InvalidIndex;
    }

    return selections_.begin()->first;
}


bool ListControlItemSelectionManager::IsIndexSelected(std::size_t index) const {

    for (const auto& each_range : selections_) {
        if (IsIndexInRange(index, each_range)) {
            return true;
        }
    }

    return false;
}


}
}