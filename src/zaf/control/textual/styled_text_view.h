#pragma once

#include <zaf/base/range.h>
#include <zaf/control/textual/ranged_text_style.h>
#include <zaf/control/textual/default_text_style.h>

namespace zaf::textual {

class StyledText;

template<typename Accessor, typename Item, typename IsRangeInView>
class ViewIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = Item;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

public:
    ViewIterator(
        const Accessor& accessor,
        const Accessor::const_iterator& accessor_iterator,
        const Range& view_range) noexcept
        :
        accessor_(accessor),
        accessor_iterator_(accessor_iterator),
        view_range_(view_range) {

    }

    ViewIterator& operator++() {
        Forward();
        return *this;
    }

    ViewIterator operator++(int) {
        auto previous_accessor_iterator = accessor_iterator_;
        Forward();
        return ViewIterator{ accessor_, previous_accessor_iterator, view_range_ };
    }

    const value_type& operator*() const {
        auto new_range = Range::MakeIntersection(accessor_iterator_->Range(), view_range_);
        new_range.index -= view_range_.index;
        current_value_.emplace(*accessor_iterator_, new_range);
        return *current_value_;
    }

    const value_type* operator->() const {
        return &this->operator*();
    }

    bool operator==(const ViewIterator& other) const {
        return this->accessor_iterator_ == other.accessor_iterator_;
    }

    bool operator!=(const ViewIterator& other) const {
        return this->accessor_iterator_ != other.accessor_iterator_;
    }

private:
    void Forward() {

        ++accessor_iterator_;

        if (accessor_iterator_ != accessor_.end() &&
            !IsRangeInView()(accessor_iterator_->Range(), view_range_)) {
            accessor_iterator_ = accessor_.end();
        }
    }

private:
    const Accessor& accessor_;
    Accessor::const_iterator accessor_iterator_;
    const Range view_range_;
    mutable std::optional<value_type> current_value_;
};


template<typename T>
class RangedItemView {
private:
    using Accessor = RangedItemAccessor<T>;

    struct IsRangeInView {
        bool operator()(const Range& range, const Range& view_range) const {
            return view_range.Intersects(range);
        }
    };

public:
    class value_type {
    public:
        value_type(const Accessor::value_type& item, const Range& range) noexcept :
            item_(item),
            range_(range) {

        }

        const zaf::Range& Range() const noexcept {
            return range_;
        }

        const T& Value() const noexcept {
            return item_.Value();
        }

    private:
        const Accessor::value_type& item_;
        const zaf::Range range_;
    };

    using iterator = ViewIterator<Accessor, value_type, IsRangeInView>;
    using const_iterator = iterator;

public:
    RangedItemView(const Accessor& accessor, const Range& view_range) noexcept :
        accessor_(accessor),
        view_range_(view_range) {
    
    }

    std::size_t Count() const noexcept {

        std::size_t count{};
        auto iterator = accessor_.FindFirstItemIntersectsWithRange(view_range_);
        for (; iterator != accessor_.end(); ++iterator) {

            if (iterator->Range().Intersects(view_range_)) {
                ++count;
            }
            else {
                break;
            }
        }
        return count;
    }

    bool IsEmpty() const noexcept {
        auto iterator = accessor_.FindFirstItemIntersectsWithRange(view_range_);
        return iterator == accessor_.end();
    }

    iterator begin() const {
        auto accessor_iterator = accessor_.FindFirstItemIntersectsWithRange(view_range_);
        if (accessor_iterator != accessor_.end()) {
            return iterator{ accessor_, accessor_iterator, view_range_ };
        }
        return this->end();
    }

    iterator end() const {
        return iterator{ accessor_, accessor_.end(), view_range_ };
    }

    const_iterator cbegin() const {
        return this->begin();
    }

    const_iterator cend() const {
        return this->end();
    }

private:
    const Accessor& accessor_;
    const Range view_range_;
};


class InlineObjectView {
private:
    struct IsRangeInView {
        bool operator()(const Range& range, const Range& view_range) const {
            return view_range.Contains(range);
        }
    };

public:
    class value_type {
    public:
        value_type(const InlineObjectAccessor::value_type& item, const Range& range) noexcept :
            item_(item),
            range_(range) {

        }

        const zaf::Range& Range() const noexcept {
            return range_;
        }

        const std::shared_ptr<InlineObject>& Object() const noexcept {
            return item_.Object();
        }

    private:
        const InlineObjectAccessor::value_type& item_;
        const zaf::Range range_;
    };

    using iterator = ViewIterator<InlineObjectAccessor, value_type, IsRangeInView>;
    using const_iterator = iterator;

public:
    InlineObjectView(const InlineObjectAccessor& accessor, const Range& range) : 
        accessor_(accessor), 
        view_range_(range) {

    }

    std::size_t Count() const noexcept {

        std::size_t count{};
        auto iterator = accessor_.FindFirstItemContainedInRange(view_range_);
        for (; iterator != accessor_.end(); ++iterator) {

            if (view_range_.Contains(iterator->Range())) {
                ++count;
            }
            else {
                break;
            }
        }
        return count;
    }

    bool IsEmpty() const noexcept {
        auto iterator = accessor_.FindFirstItemContainedInRange(view_range_);
        return iterator == accessor_.end();
    }

    iterator begin() const {
        auto accessor_iterator = accessor_.FindFirstItemContainedInRange(view_range_);
        if (accessor_iterator != accessor_.end()) {
            return iterator{ accessor_, accessor_iterator, view_range_ };
        }
        return this->end();
    }

    iterator end() const {
        return iterator{ accessor_, accessor_.end(), view_range_ };
    }

    const_iterator cbegin() const {
        return this->begin();
    }

    const_iterator cend() const {
        return this->end();
    }

private:
    const InlineObjectAccessor& accessor_;
    const Range view_range_;
};


class StyledTextView {
public:
    using RangedFontView = RangedItemView<Font>;
    using RangedColorView = RangedItemView<Color>;

public:
    StyledTextView(const StyledText& styled_text, const Range& view_range);

    std::wstring_view Text() const noexcept;

    const DefaultTextStyle& DefaultStyle() const noexcept;

    const Font& DefaultFont() const noexcept;
    const Font& GetFontAtIndex(std::size_t index) const;
    RangedFontView RangedFonts() const noexcept;

    const Color& DefaultTextColor() const noexcept;
    const Color& GetTextColorAtIndex(std::size_t index) const;
    RangedColorView RangedTextColors() const noexcept;

    const Color& DefaultTextBackColor() const noexcept;
    const Color& GetTextBackColorAtIndex(std::size_t index) const;
    RangedColorView RangedTextBackColors() const noexcept;

    std::shared_ptr<InlineObject> GetInlineObjectAtIndex(std::size_t index) const;
    InlineObjectView InlineObjects() const noexcept;

private:
    const StyledText& styled_text_;
    Range view_range_;
};

}