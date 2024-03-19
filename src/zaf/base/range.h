#pragma once

#include <dwrite.h>
#include <Richedit.h>
#include <zaf/base/error/check.h>
#include <zaf/base/hash.h>
#include <zaf/object/equality.h>
#include <zaf/object/object.h>

namespace zaf {

/**
Describes a range using start index and length.
*/
class Range : public Object {
public:
    ZAF_DECLARE_TYPE;
    ZAF_DECLARE_EQUALITY;

    /**
    Gets an infinite range whose start index is 0 and length is the max value.
    */
    static Range Infinite() {
        return Range{ 0, (std::numeric_limits<std::size_t>::max)() };
    }

    /**
    Creates a range with a pair of start and end indexes. The end index is not included in the 
    range.

    @throw std::logic_error
        Thrown if start_index is greater than end_index.
    */
    static Range FromIndexPair(std::size_t start_index, std::size_t end_index) {
        ZAF_EXPECT(start_index <= end_index);
        return Range{ start_index, end_index - start_index };
    }

    /**
    Creates a range from a DWRITE_TEXT_RANGE structure.
    */
    static Range FromDWRITETEXTRANGE(const DWRITE_TEXT_RANGE& text_range) {
        return Range{ text_range.startPosition, text_range.length };
    }

    /**
    Creates a range from a CHARRANGE structure.
    */
    static Range FromCHARRANGE(const CHARRANGE& char_range) {

        if (char_range.cpMin == 0 && char_range.cpMax == -1) {
            return Range::Infinite();
        }

        return FromIndexPair(char_range.cpMin, char_range.cpMax);
    }

    /**
    Makes an intersection range of two ranges.

    @return 
        The intersection range. If there is no intersection between the two ranges, the length of 
        the result is zero.
    */
    static Range MakeIntersection(const Range& range1, const Range& range2) {
        auto start_index = (std::max)(range1.index, range2.index);
        auto end_index = (std::min)(range1.EndIndex(), range2.EndIndex());
        auto intersection_length = (start_index <= end_index) ? (end_index - start_index) : 0;
        return Range{ start_index, intersection_length };
    }

public:
    Range() { }
    Range(std::size_t index, std::size_t length) : index(index), length(length) { }

    std::size_t Index() const {
        return index;
    }

    void SetIndex(std::size_t index) {
        this->index = index;
    }

    std::size_t Length() const {
        return length;
    }

    void SetLength(std::size_t length) {
        this->length = length;
    }

    /**
    Gets the end index, which is next to the last index in range, and is not included in the range.
    */
    std::size_t EndIndex() const {
        return index + length;
    }

    /**
    Determines whether the current range is empty.

    @return
        True if the range is empty, meaning it has zero length; otherwise, false.
    */
    bool IsEmpty() const noexcept {
        return length == 0;
    }

    /**
    Determines whether the specified index is in the range.
    */
    bool Contains(std::size_t index) const noexcept {
        return (Index() <= index) && (index < EndIndex());
    }

    /**
    Determines whether the current range fully contains the other range.
    */
    bool Contains(const Range& other) const noexcept {
        if (other.length == 0) {
            return Contains(other.index);
        }
        return (Index() <= other.Index()) && (other.EndIndex() <= EndIndex());
    }

    /**
    Determines whether the current range intersects with the other range.

    @return
        Returns true if the current range intersects with the other range, otherwise false.
        A zero length range doesn't intersect with others.
    */
    bool Intersects(const Range& other) const noexcept {
        return MakeIntersection(*this, other).length > 0;
    }

    /**
    Converts the range to a DWRITE_TEXT_RANGE structure.
    */
    DWRITE_TEXT_RANGE ToDWRITETEXTRANGE() const {
        DWRITE_TEXT_RANGE text_range;
        text_range.startPosition = static_cast<UINT32>(index);
        text_range.length = static_cast<UINT32>(length);
        return text_range;
    }

    /**
    Converts the range to a CHARRANGE structure.
    */
    CHARRANGE ToCHARRANGE() const {
        CHARRANGE result;
        if (index == 0 && length == (std::numeric_limits<std::size_t>::max)()) {
            result.cpMin = 0;
            result.cpMax = -1;
        }
        else {
            result.cpMin = static_cast<LONG>(index);
            result.cpMax = static_cast<LONG>(EndIndex());
        }
        return result;
    }

    std::wstring ToString() const override;

    friend bool operator==(const Range& range1, const Range& range2) {
        return std::tie(range1.index, range1.length) == std::tie(range2.index, range2.length);
    }

    friend auto operator<=>(const Range& range1, const Range& range2) {
        return std::tie(range1.index, range1.length) <=> std::tie(range2.index, range2.length);
    }

public:
    std::size_t index{};
    std::size_t length{};

public:
    /**
    An iterator used to loop over all indexes in a range.
    */
    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = std::size_t;
        using difference_type = std::make_signed_t<std::size_t>;
        using pointer = std::size_t*;
        using reference = std::size_t&;

    public:
        Iterator(value_type current, value_type end) : current_(current), end_(end) { }

        Iterator& operator++() {
            Advance();
            return *this;
        }

        Iterator operator++(int) {
            auto previous = current_;
            Advance();
            return Iterator{ previous, end_ };
        }

        value_type operator*() const {
            return current_;
        }

        bool operator!=(const Iterator& other) const {
            return this->current_ != other.current_;
        }

    private:
        void Advance() {
            if (current_ != end_) {
                ++current_;
            }
        }

    private:
        value_type current_{};
        const value_type end_{};
    };

    Iterator begin() const {
        return Iterator{ index, EndIndex()};
    }

    Iterator end() const {
        return Iterator{ EndIndex(), EndIndex() };
    }
};

}


namespace std {
template<>
struct hash<zaf::Range> {
    std::size_t operator()(const zaf::Range& range) {
        return zaf::CalculateHash(range.index, range.length);
    }
};
}