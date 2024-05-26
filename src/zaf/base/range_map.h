#pragma once

#include <vector>
#include <zaf/base/range.h>

namespace zaf {

template<typename T>
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
    
    void Clear() noexcept {
        items_.clear();
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
                if (new_value && *new_value == iterator->Value()) {

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
                if (new_value && *new_value == iterator->Value()) {
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
                if (new_value && *new_value == iterator->Value()) {
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
                if (new_value && *new_value == iterator->Value()) {
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
                if (new_value && *new_value == iterator->Value()) {

                    iterator->range_.length += replaced_range.length;
                    has_merged_new_value = true;

                    //Merge other trailing continous ranges.
                    auto remove_begin = iterator + 1;
                    auto remove_end = remove_begin;
                    while (remove_end != items_.end()) {

                        if (remove_end->Range().index > iterator->Range().EndIndex() ||
                            !(remove_end->Value() == iterator->Value())) {
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