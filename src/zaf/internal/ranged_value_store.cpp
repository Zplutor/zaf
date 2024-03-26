#include <zaf/internal/ranged_value_store.h>

namespace zaf {
namespace internal {

bool RangedValueStore::AddRange(const Range& new_range, std::any value) {
    return ReplaceRange(new_range, std::move(value));
}


bool RangedValueStore::AddRange(std::size_t index, std::size_t length) {
    return AddRange(Range{ index, length }, {});
}


bool RangedValueStore::RemoveRange(const Range& removed_range) {
    return ReplaceRange(removed_range, std::nullopt);
}


bool RangedValueStore::RemoveRange(std::size_t index, std::size_t length) {
    return RemoveRange(Range{ index, length });
}


bool RangedValueStore::ReplaceRange(const Range& replaced_range, std::optional<std::any> value) {

    //Ignore empty ranges.
    if (replaced_range.length == 0) {
        return false;
    }

    bool range_changed{};

    //Loop over all items, to find insert position and modify existent ranges.
    auto iterator = items_.begin();
    while (iterator != items_.end()) {

        const auto current_range = iterator->Range();

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
            *iterator = Item{
                Range{ current_range.index, replaced_range.index - current_range.index },
                std::move(iterator->Value())
            };

            //Insert the latter sub range.
            Item latter_item{
                Range{ 
                    replaced_range.EndIndex(),
                    current_range.EndIndex() - replaced_range.EndIndex() 
                },
                iterator->Value()
            };

            ++iterator;
            iterator = items_.insert(iterator, std::move(latter_item));
            range_changed = true;
            break;
        }

        //The tail of the new range is overlapped with the head of the current range, the current
        //range should be modified.
        if ((replaced_range.index <= current_range.index) &&
            (replaced_range.EndIndex() > current_range.index)) {

            Item modified_item{
                Range{ 
                    replaced_range.EndIndex(), 
                    current_range.EndIndex() - replaced_range.EndIndex() 
                },
                std::move(iterator->Value())
            };

            *iterator = std::move(modified_item);
            range_changed = true;
            break;
        }

        //The head of the new range is overlapped with the tail of the current range, the current
        //range should be modified.
        if ((replaced_range.index < current_range.EndIndex()) &&
            (replaced_range.EndIndex() >= current_range.EndIndex())) {

            *iterator = Item{
                Range{ current_range.index, replaced_range.index - current_range.index },
                std::move(iterator->Value())
            };

            ++iterator;
            range_changed = true;
            continue;
        }

        ++iterator;
    }

    //Insert the new item to the position found.
    if (value) {
        items_.insert(iterator, Item{ replaced_range, std::move(*value) });
        range_changed = true;
    }

    return range_changed;
}


bool RangedValueStore::InsertSpan(const Range& span_range) {

    //Ignore empty spans.
    if (span_range.length == 0) {
        return false;
    }

    return ReplaceSpan(Range{ span_range.index, 0 }, span_range.length);
}


bool RangedValueStore::EraseSpan(const Range& span_range) {

    //Ignore empty spans.
    if (span_range.length == 0) {
        return false;
    }

    return ReplaceSpan(span_range, 0);
}


bool RangedValueStore::ReplaceSpan(const Range& span_range, std::size_t new_length) {

    bool range_changed{};

    //Loop over all items to modify ranges.
    auto iterator = items_.begin();
    while (iterator != items_.end()) {

        const auto current_range = iterator->Range();

        //The span is replaced before the current range, modify the index of the current range.
        if (span_range.EndIndex() <= current_range.index) {

            std::size_t new_index = current_range.index;
            new_index -= span_range.length;
            new_index += new_length;

            *iterator = Item{
                Range{ new_index, current_range.length },
                std::move(iterator->Value())
            };

            ++iterator;
            range_changed = true;
            continue;
        }

        //The current range is fully contained by(or equals to) the replaced span, remove the 
        //current range.
        if (span_range.Contains(current_range)) {

            iterator = items_.erase(iterator);
            range_changed = true;
            continue;
        }

        //The replaced span is a portion of the current range.
        if ((span_range.index > current_range.index) &&
            (span_range.EndIndex() < current_range.EndIndex())) {

            //The new length is zero, decline the length of the current range.
            if (new_length == 0) {

                *iterator = Item{
                    Range{ current_range.index, current_range.length - span_range.length },
                    std::move(iterator->Value())
                };

                ++iterator;
                range_changed = true;
            }
            //Otherwise, break the current range.
            else {

                //Shorten the length of the head sub range.
                *iterator = Item{
                    Range{ current_range.index, span_range.index - current_range.index },
                    std::move(iterator->Value())
                };

                //Insert the new tail sub range.
                Item tail_item{
                    Range{
                        span_range.index + new_length,
                        current_range.EndIndex() - span_range.EndIndex()
                    },
                    iterator->Value()
                };

                ++iterator;
                iterator = items_.insert(iterator, std::move(tail_item));
                ++iterator;
                range_changed = true;
            }
            continue;
        }

        //The replaced span is overlapped with the head of the current range, modify the index of
        //the current range.
        if ((span_range.EndIndex() > current_range.index) &&
            (span_range.EndIndex() < current_range.EndIndex())) {

            *iterator = Item{
                Range{
                    span_range.index + new_length, 
                    current_range.EndIndex() - span_range.EndIndex() 
                },
                std::move(iterator->Value())
            };

            ++iterator;
            range_changed = true;
            continue;
        }

        //The replaced span is overlapped with the tail of the current range, modify the length of
        //the current span.
        if ((span_range.index > current_range.index) &&
            (span_range.index < current_range.EndIndex())) {

            *iterator = Item{
                Range{
                    current_range.index,
                    current_range.length - (current_range.EndIndex() - span_range.index)
                },
                std::move(iterator->Value())
            };

            ++iterator;
            range_changed = true;
            continue;
        }

        ++iterator;
    }

    return range_changed;
}


const RangedValueStore::Item* RangedValueStore::FindItemContainsIndex(
    std::size_t index) const noexcept {

    auto iterator = std::lower_bound(
        items_.begin(),
        items_.end(),
        index,
        [](const Item& item, std::size_t index) {
            return item.Range().EndIndex() <= index;
        });

    if (iterator != items_.end() && iterator->Range().Contains(index)) {
        return &*iterator;
    }

    return nullptr;
}


RangedValueStore::Item* RangedValueStore::FindItemContainsIndex(std::size_t index) noexcept {
    const RangedValueStore* const_this = this;
    return const_cast<RangedValueStore::Item*>(const_this->FindItemContainsIndex(index));
}

}
}