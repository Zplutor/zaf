#pragma once

#include <zaf/base/none.h>
#include <zaf/base/range_map.h>

namespace zaf::internal {

class RangeSet {
private:
    using UnderlyingStore = RangeMap<None>;

public:
    using value_type = Range;

    class iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Range;
        using difference_type = std::make_signed_t<std::size_t>;
        using pointer = const Range*;
        using reference = const Range&;

    public:
        explicit iterator(UnderlyingStore::const_iterator inner) : inner_(inner) {

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
        UnderlyingStore::const_iterator inner_;
    };

    using const_iterator = iterator;

public:
    void AddRange(const Range& range) {
        store_.AddRange(range, {});
    }

    void RemoveRange(const Range& range) {
        store_.RemoveRange(range);
    }

    void InsertSpan(const Range& span_range) {
        store_.InsertSpan(span_range);
    }

    void EraseSpan(const Range& span_range) {
        store_.EraseSpan(span_range);
    }

    void Clear() {
        store_.Clear();
    }

    bool ContainsIndex(std::size_t index) const noexcept {
        return store_.FindItemAtIndex(index) != store_.end();
    }

    bool IsEmpty() const noexcept {
        return store_.IsEmpty();
    }

    std::size_t Count() const noexcept {
        return store_.Count();
    }

    iterator begin() const {
        return iterator{ store_.begin() };
    }

    iterator end() const {
        return iterator{ store_.end() };
    }

private:
    UnderlyingStore store_;
};

}