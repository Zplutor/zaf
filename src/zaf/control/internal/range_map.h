#pragma once

#include <zaf/internal/ranged_value_store.h>

namespace zaf {
namespace internal {

template<typename ValueType>
class RangeMap {
public:
    bool AddRange(const Range& range, ValueType value) {
        return range_manager_.AddRange(range, std::move(value));
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

    bool ReplaceSpan(const Range& span_range, std::size_t new_length) {
        return range_manager_.ReplaceSpan(span_range, new_length);
    }

    void Clear() {
        range_manager_.Clear();
    }

    const ValueType* GetValueAtIndex(std::size_t index) const {

        auto item = range_manager_.FindItemContainsIndex(index);
        if (item) {
            return std::any_cast<ValueType>(&item->Value());
        }
        return nullptr;
    }

public:
    class Item {
    public:
        explicit Item(const RangedValueStore::Item& item) : inner_item_(item) {

        }

        const zaf::Range& Range() const {
            return inner_item_.Range();
        }

        const ValueType& Value() const {
            return *std::any_cast<ValueType>(&inner_item_.Value());
        }

    private:
        const RangedValueStore::Item& inner_item_;
    };

    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Item;
        using difference_type = std::make_signed_t<std::size_t>;
        using pointer = Item*;
        using reference = Item&;

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

        value_type operator*() const {
            return Item{ *inner_ };
        }

        bool operator!=(const Iterator& other) const {
            return inner_ != other.inner_;
        }

    private:
        RangedValueStore::ItemList::const_iterator inner_;
    };

    Iterator begin() const {
        return Iterator{ range_manager_.begin() };
    }

    Iterator end() const {
        return Iterator{ range_manager_.end() };
    }

    using iterator = Iterator;
    using const_iterator = Iterator;

private:
    RangedValueStore range_manager_;
};

}
}