#pragma once

#include <zaf/base/range.h>
#include <zaf/internal/ranged_value_store.h>

namespace zaf::internal {

class RangeSet {
public:
    bool AddRange(const Range& range) {
        return range_manager_.AddRange(range, {});
    }

    bool RemoveRange(const Range& range) {
        return range_manager_.RemoveRange(range);
    }

    bool InsertSpan(const Range& span_range) {
        return range_manager_.InsertSpan(span_range);
    }

    bool EraseSpan(const Range& span_range) {
        return range_manager_.EraseSpan(span_range);
    }

    void Clear() {
        range_manager_.Clear();
    }

    bool ContainsIndex(std::size_t index) const {
        return !!range_manager_.FindItemContainsIndex(index);
    }

    bool IsEmpty() const {
        return range_manager_.Items().empty();
    }

    std::size_t Count() const {
        return range_manager_.Items().size();
    }

    const Range& operator[](std::size_t index) const {
        return range_manager_.Items()[index].Range();
    }

public:
    /**
    An iterator used to loop over all ranges in the RangeSet.
    */
    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Range;
        using difference_type = std::make_signed_t<std::size_t>;
        using pointer = Range*;
        using reference = Range&;

    public:
        explicit Iterator(RangedValueStore::ItemList::const_iterator inner) : 
            inner_(inner) {

        }

        Iterator& operator++() {
            inner_++;
            return *this;
        }

        Iterator operator++(int) {
            return Iterator{ ++inner_ };
        }

        const value_type& operator*() const {
            return inner_->Range();
        }

        bool operator!=(const Iterator& other) const {
            return inner_ != other.inner_;
        }

    private:
        RangedValueStore::ItemList::const_iterator inner_;
    };

    Iterator begin() const {
        return Iterator{ range_manager_.Items().begin() };
    }

    Iterator end() const {
        return Iterator{ range_manager_.Items().end() };
    }

private:
    RangedValueStore range_manager_;
};

}