#pragma once

#include <algorithm>
#include <zaf/base/error/check.h>

namespace zaf {
namespace internal {

template<typename T>
class Range {
public:
    using value_type = T;
    using difference_type = std::make_signed_t<T>;

    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = difference_type;
        using pointer = T*;
        using reference = T&;

    public:
        Iterator(value_type current_value, value_type end_value, difference_type step) : 
            current_value_(current_value),
            end_value_(end_value),
            step_(step) {

        }

        Iterator& operator++() {
            Advance();
            return *this;
        }

        Iterator operator++(int) {
            auto previous_value = current_value_;
            Advance();
            return Iterator(previous_value, end_value_, step_);
        }

        value_type operator*() const {
            return current_value_;
        }

        bool operator!=(const Iterator& other) const {
            return this->current_value_ != other.current_value_;
        }

    private:
        void Advance() {

            current_value_ += step_;

            if (step_ < 0) {
                current_value_ = (std::max)(current_value_, end_value_);
            }
            else {
                current_value_ = (std::min)(current_value_, end_value_);
            }
        }

    private:
        value_type current_value_{};
        const value_type end_value_{};
        const difference_type step_{};
    };

public:
    Range(T begin_value, T end_value, difference_type step) : 
        begin_value_(begin_value),
        end_value_(end_value),
        step_(step) {

    }

    Iterator begin() const {
        return Iterator{ begin_value_, end_value_, step_ };
    }

    Iterator end() const {
        return Iterator{ end_value_, end_value_, step_ };
    }

private:
    const value_type begin_value_{};
    const value_type end_value_{};
    const difference_type step_{};
};

}

template<typename T>
internal::Range<T> Range(T begin, T end, typename internal::Range<T>::difference_type step = 1) {

    if (step < 0) {
        ZAF_EXPECT(begin >= end);
    }
    else {
        ZAF_EXPECT(begin <= end);
    }

    return internal::Range<T>{ begin, end, step };
}


template<typename T>
internal::Range<T> Range(T end) {
    return internal::Range<T>{ 0, end, 1 };
}

}