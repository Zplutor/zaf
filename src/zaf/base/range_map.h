#pragma once

#include <functional>
#include <vector>
#include <zaf/base/range.h>

namespace zaf {

template<typename T, typename Equal = std::equal_to<T>>
class RangeMap {
public:
    class Item {
    public:
        Item(const Range& range, T value) noexcept : range_(range), value_(std::move(value)) {

        }

        const Range& Range() const noexcept {
            return range_;
        }

        const T& Value() const noexcept {
            return value_;
        }

        T& Value() noexcept {
            return value_;
        }

    private:
        //Allow RangeMap to modify the private members.
        friend RangeMap;

        zaf::Range range_;
        T value_;
    };

    using ItemList = std::vector<Item>;

    using value_type = Item;
    using iterator = ItemList::iterator;
    using const_iterator = ItemList::const_iterator;

public:
    void AddRange(const Range& added_range, T value) {
        ReplaceRange(added_range, &value);
    }

    void RemoveRange(const Range& removed_range) {
        ReplaceRange(removed_range, nullptr);
    }

    void InsertSpan(const Range& span_range) {
        if (!span_range.IsEmpty()) {
            ReplaceSpan(Range{ span_range.index, 0 }, span_range.length);
        }
    }

    void EraseSpan(const Range& span_range) {
        if (!span_range.IsEmpty()) {
            ReplaceSpan(span_range, 0);
        }
    }

    void ReplaceSpan(const Range& span_range, std::size_t new_length) {

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
                continue;
            }

            //The current range is fully contained by(or equals to) the replaced span, remove the 
            //current range.
            if (span_range.Contains(current_range)) {

                iterator = items_.erase(iterator);
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
                continue;
            }

            ++iterator;
        }
    }
    
    void Clear() noexcept {
        items_.clear();
    }

    iterator FindItemAtIndex(std::size_t index) noexcept {

        auto iterator = std::lower_bound(
            items_.begin(),
            items_.end(),
            index,
            [](const Item& item, std::size_t index) {
                return item.Range().EndIndex() <= index;
            });

        if (iterator != items_.end() && iterator->Range().Contains(index)) {
            return iterator;
        }

        return items_.end();
    }

    const_iterator FindItemAtIndex(std::size_t index) const noexcept {
        auto mutable_this = const_cast<RangeMap<T>*>(this);
        return mutable_this->FindItemAtIndex(index);
    }

    iterator FindFirstItemIntersectsWithRange(const Range& range) noexcept {

        auto iterator = std::lower_bound(
            items_.begin(),
            items_.end(),
            range,
            [](const Item& item, const Range& range) {
                return item.Range().EndIndex() < range.index;
            });

        if (iterator != items_.end() && iterator->Range().Intersects(range)) {
            return iterator;
        }

        return items_.end();
    }

    const_iterator FindFirstItemIntersectsWithRange(const Range& range) const noexcept {
        auto mutable_this = const_cast<RangeMap<T>*>(this);
        return mutable_this->FindFirstItemIntersectsWithRange(range);
    }

    bool IsEmpty() const noexcept {
        return items_.empty();
    }

    std::size_t Count() const noexcept {
        return items_.size();
    }

    iterator begin() noexcept {
        return items_.begin();
    }

    iterator end() noexcept {
        return items_.end();
    }

    const_iterator begin() const noexcept {
        return items_.begin();
    }

    const_iterator end() const noexcept {
        return items_.end();
    }

    const_iterator cbegin() const noexcept {
        return items_.cbegin();
    }

    const_iterator cend() const noexcept {
        return items_.cend();
    }

private:
    void ReplaceRange(const Range& replaced_range, T* const new_value) {

        if (replaced_range.length == 0) {
            return;
        }

        bool has_merged_new_value{};

        //Loop over all items, to find insert position and modify existent ranges.
        auto iterator = items_.begin();
        while (iterator != items_.end()) {

            const auto current_range = iterator->Range();

            //The beginning of the current range is greater than the end of the new range,
            //insert position is found.
            if (replaced_range.EndIndex() < current_range.index) {
                break;
            }

            //The current range is right after the new range, modify the current range to contain
            //the new range.
            if (replaced_range.EndIndex() == current_range.index) {
                if (new_value && Equal{}(*new_value, iterator->Value())) {

                    iterator->range_ = Range::FromIndexPair(
                        replaced_range.index, 
                        current_range.EndIndex());

                    has_merged_new_value = true;
                }
                break;
            }

            //The new range contains(or equals to) the current range, the current range should be
            //removed.
            if (replaced_range.Contains(current_range)) {
                iterator = items_.erase(iterator);
                continue;
            }

            //The new range is inclusively contained by the current range.
            if (replaced_range.index > current_range.index &&
                replaced_range.EndIndex() < current_range.EndIndex()) {

                //If the values are equal, nothing need to be done.
                if (new_value && Equal{}(*new_value, iterator->Value())) {
                    has_merged_new_value = true;
                    break;
                }

                //Otherwise, break the current range into two sub ranges.
                //Modify the current range to the first sub range.
                iterator->range_ = Range::FromIndexPair(current_range.index, replaced_range.index);

                //Insert the latter sub range.
                Item latter_item{
                    Range::FromIndexPair(replaced_range.EndIndex(), current_range.EndIndex()),
                    iterator->Value()
                };

                ++iterator;
                iterator = items_.insert(iterator, std::move(latter_item));
                break;
            }

            //The tail of the new range is overlapped with the head of the current range, the 
            //current range should be modified.
            if ((replaced_range.index <= current_range.index) &&
                (replaced_range.EndIndex() > current_range.index)) {

                Range modified_range;

                //If the values are equal, merge two ranges.
                if (new_value && Equal{}(*new_value, iterator->Value())) {
                    modified_range.index = replaced_range.index;
                    modified_range.length = current_range.EndIndex() - replaced_range.index;
                    has_merged_new_value = true;
                }
                //Otherwise narrow the current range.
                else {
                    modified_range.index = replaced_range.EndIndex();
                    modified_range.length = current_range.EndIndex() - replaced_range.EndIndex();
                }

                iterator->range_ = modified_range;
                break;
            }

            //The head of the new range is overlapped with the tail of the current range, the 
            //current range should be modified.
            if ((replaced_range.index < current_range.EndIndex()) &&
                (replaced_range.EndIndex() >= current_range.EndIndex())) {

                Range modified_range;
                modified_range.index = current_range.index;

                //If the values are equal, merge two ranges.
                if (new_value && Equal{}(*new_value, iterator->Value())) {
                    modified_range.length = replaced_range.EndIndex() - current_range.index;
                }
                //Otherwise narrow the current range.
                else {
                    modified_range.length = replaced_range.index - current_range.index;
                }

                iterator->range_ = modified_range;
                ++iterator;
                continue;
            }

            //The new range is right after the current range, modify the current range to contain
            //the new range.
            if (current_range.EndIndex() == replaced_range.index) {
                if (new_value && Equal{}(*new_value, iterator->Value())) {

                    iterator->range_.length += replaced_range.length;
                    has_merged_new_value = true;

                    //Merge other trailing continous ranges.
                    auto remove_begin = iterator + 1;
                    auto remove_end = remove_begin;
                    while (remove_end != items_.end()) {

                        if (remove_end->Range().index > iterator->Range().EndIndex() ||
                            !Equal{}(remove_end->Value(), iterator->Value())) {
                            break;
                        }

                        if (remove_end->Range().EndIndex() > iterator->Range().EndIndex()) {
                            iterator->range_.length = 
                                remove_end->Range().EndIndex() - iterator->Range().index;
                        }

                        ++remove_end;
                    }
                    items_.erase(remove_begin, remove_end);
                    ++iterator;
                    break;
                }
            }

            ++iterator;
        }

        //Insert the new item to the position found.
        if (new_value && !has_merged_new_value) {
            items_.insert(iterator, Item{ replaced_range, std::move(*new_value) });
        }
    }

private:
    ItemList items_;
};

}