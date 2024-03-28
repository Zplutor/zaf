#pragma once

#include <zaf/base/range.h>
#include <zaf/internal/ranged_value_store.h>

namespace zaf::internal {

class RangeSet {
public:
    using value_type = Range;

    class iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Range;
        using difference_type = std::make_signed_t<std::size_t>;
        using pointer = Range*;
        using reference = Range&;

    public:
        explicit iterator(RangedValueStore::const_iterator inner) : inner_(inner) {

        }

        iterator& operator++() {
            inner_++;
            return *this;
        }

        iterator operator++(int) {
            return iterator{ ++inner_ };
        }

        const value_type& operator*() const {
            return inner_->Range();
        }

        const value_type* operator->() const {
            return &inner_->Range();
        }

        bool operator!=(const iterator& other) const {
            return inner_ != other.inner_;
        }

    private:
        RangedValueStore::const_iterator inner_;
    };

    using const_iterator = iterator;

public:
    bool AddRange(const Range& range) {
        return store_.AddRange(range, {});
    }

    bool RemoveRange(const Range& range) {
        return store_.RemoveRange(range);
    }

    bool InsertSpan(const Range& span_range) {
        return store_.InsertSpan(span_range);
    }

    bool EraseSpan(const Range& span_range) {
        return store_.EraseSpan(span_range);
    }

    void Clear() {
        store_.Clear();
    }

    bool ContainsIndex(std::size_t index) const noexcept {
        return !!store_.FindItemContainsIndex(index);
    }

    bool IsEmpty() const noexcept {
        return store_.IsEmpty();
    }

    std::size_t Count() const noexcept {
        return store_.Count();
    }

public:
    iterator begin() const {
        return iterator{ store_.begin() };
    }

    iterator end() const {
        return iterator{ store_.end() };
    }

private:
    RangedValueStore store_;
};

}