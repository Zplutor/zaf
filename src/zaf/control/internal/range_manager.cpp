#include <zaf/control/internal/range_manager.h>

namespace zaf {
namespace internal {

bool RangeManager::AddRange(const Range& new_range, std::any value) {
    return ReplaceRange(new_range, std::move(value));
}


bool RangeManager::AddRange(std::size_t index, std::size_t length) {
    return AddRange(Range{ index, length }, {});
}


bool RangeManager::RemoveRange(const Range& removed_range) {
    return ReplaceRange(removed_range, std::nullopt);
}


bool RangeManager::RemoveRange(std::size_t index, std::size_t length) {
    return RemoveRange(Range{ index, length });
}


void RangeManager::RemoveRangesIntersectWith(const Range& range) {

    auto iterator = items_.begin();
    while (iterator != items_.end()) {

        const auto& current_range = iterator->range;

        if (range.EndIndex() <= current_range.index || current_range.EndIndex() <= range.index) {
            ++iterator;
        }
        else {
            iterator = items_.erase(iterator);
        }
    }
}


bool RangeManager::ReplaceRange(const Range& replaced_range, std::optional<std::any> value) {

    //Ignore empty ranges.
    if (replaced_range.length == 0) {
        return false;
    }

    bool range_changed{};

    //Loop over all items, to find insert position and modify existent ranges.
    auto iterator = items_.begin();
    while (iterator != items_.end()) {

        const auto current_range = iterator->range;

        //The beginning of the current range is greater than the end of the new range,
        //insert position is found.
        if (replaced_range.EndIndex() <= current_range.index) {
            break;
        }

        //The new range contains(or equals to) the current range, the current range should be
        //removed.
        if (replaced_range.Contains(current_range)) {
            iterator = items_.erase(iterator);
            range_changed = true;
            continue;
        }

        //The new range is inclusively contained by the current range, the current range should be 
        //broken into two sub ranges.
        if (replaced_range.index > current_range.index &&
            replaced_range.EndIndex() < current_range.EndIndex()) {

            //Modify current range to the first sub range.
            iterator->range.length = replaced_range.index - current_range.index;

            //Insert the latter sub range.
            Item latter_item;
            latter_item.range.index = replaced_range.EndIndex();
            latter_item.range.length = current_range.EndIndex() - replaced_range.EndIndex();
            latter_item.value = iterator->value;

            ++iterator;
            iterator = items_.insert(iterator, std::move(latter_item));
            range_changed = true;
            break;
        }

        //The tail of the new range is overlapped with the head of the current range, the current
        //range should be modified.
        if ((replaced_range.index <= current_range.index) &&
            (replaced_range.EndIndex() > current_range.index)) {

            iterator->range.index = replaced_range.EndIndex();
            iterator->range.length = current_range.EndIndex() - replaced_range.EndIndex();
            range_changed = true;
            break;
        }

        //The head of the new range is overlapped with the tail of the current range, the current
        //range should be modified.
        if ((replaced_range.index < current_range.EndIndex()) &&
            (replaced_range.EndIndex() >= current_range.EndIndex())) {

            iterator->range.length = replaced_range.index - current_range.index;
            ++iterator;
            range_changed = true;
            continue;
        }

        ++iterator;
    }

    //Insert the new item to the position found.
    if (value) {
        Item new_item;
        new_item.range = replaced_range;
        new_item.value = std::move(*value);
        items_.insert(iterator, std::move(new_item));
        range_changed = true;
    }

    return range_changed;
}


bool RangeManager::InsertSpan(const Range& span_range) {

    //Ignore empty spans.
    if (span_range.length == 0) {
        return false;
    }

    bool range_changed{};

    //Loop over all items to modify ranges.
    auto iterator = items_.begin();
    while (iterator != items_.end()) {

        const auto current_range = iterator->range;

        //The span is inserted before or at the current range, advance the current range.
        if (span_range.index <= current_range.index) {

            iterator->range.index += span_range.length;
            range_changed = true;
        }
        //The span is inserted at the middle of the current range, break the current range.
        else if (span_range.index < current_range.EndIndex()) {

            //Shorten the length of the head sub range.
            iterator->range.length = span_range.index - current_range.index;

            //Insert the new tail sub range.
            Item tail_item;
            tail_item.range.index = span_range.EndIndex();
            tail_item.range.length = current_range.EndIndex() - span_range.index;
            tail_item.value = iterator->value;

            ++iterator;
            iterator = items_.insert(iterator, std::move(tail_item));
            range_changed = true;
        }

        ++iterator;
    }

    return range_changed;
}


bool RangeManager::EraseSpan(const Range& span_range) {

    //Ignore empty spans.
    if (span_range.length == 0) {
        return false;
    }

    bool range_changed{};

    //Loop over all items to modify ranges.
    auto iterator = items_.begin();
    while (iterator != items_.end()) {

        const auto current_range = iterator->range;

        //The span is erased before the current range, retreat the current range.
        if (span_range.EndIndex() <= current_range.index) {

            iterator->range.index -= span_range.length;
            ++iterator;
            range_changed = true;
            continue;
        }

        //The current range is fully contained by(or equals to) the erased span, remove the current
        //range.
        if (span_range.Contains(current_range)) {

            iterator = items_.erase(iterator);
            range_changed = true;
            continue;
        }

        //The erased span is a portion of the current range, decline the length of the current 
        //range.
        if ((span_range.index > current_range.index) &&
            (span_range.EndIndex() < current_range.EndIndex())) {

            iterator->range.length -= span_range.length;
            ++iterator;
            range_changed = true;
            continue;
        }

        //The erased span is overlapped with the head of the current range, modify the current 
        //range.
        if ((span_range.EndIndex() > current_range.index) &&
            (span_range.EndIndex() < current_range.EndIndex())) {

            iterator->range.index = span_range.index;
            iterator->range.length = current_range.EndIndex() - span_range.EndIndex();
            ++iterator;
            range_changed = true;
            continue;
        }

        //The erased span is overlapped with the tail of the current range, modify the current 
        //span.
        if ((span_range.index > current_range.index) &&
            (span_range.index < current_range.EndIndex())) {

            iterator->range.length -= current_range.EndIndex() - span_range.index;
            ++iterator;
            range_changed = true;
            continue;
        }

        ++iterator;
    }

    return range_changed;
}


void RangeManager::Clear() {
    items_.clear();
}


const RangeManager::Item* RangeManager::FindItemContainsIndex(std::size_t index) const {

    for (const auto& each_item : items_) {

        if (each_item.range.Contains(index)) {
            return &each_item;
        }
    }

    return nullptr;
}


}
}