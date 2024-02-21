#pragma once

#include <zaf/control/internal/range_manager.h>

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

    void Clear() {
        range_manager_.Clear();
    }

    const ValueType* GetValueAtIndex(std::size_t index) const {

        auto item = range_manager_.FindItemContainsIndex(index);
        if (item) {
            return std::any_cast<ValueType>(&item->value);
        }
        return nullptr;
    }

public:
    class Item {
    public:
        explicit Item(const RangeManager::Item& item) : inner_item_(item) {

        }

        const zaf::Range& Range() const {
            return inner_item_.range;
        }

        const ValueType& Value() const {
            return *std::any_cast<ValueType>(&inner_item_.value);
        }

    private:
        const RangeManager::Item& inner_item_;
    };

    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Item;
        using difference_type = std::make_signed_t<std::size_t>;
        using pointer = Item*;
        using reference = Item&;

    public:
        explicit Iterator(RangeManager::ItemList::const_iterator inner) :
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
        RangeManager::ItemList::const_iterator inner_;
    };

    Iterator begin() const {
        return Iterator{ range_manager_.Items().begin() };
    }

    Iterator end() const {
        return Iterator{ range_manager_.Items().end() };
    }

    using iterator = Iterator;

private:
    RangeManager range_manager_;
};

}
}