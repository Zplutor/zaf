#pragma once

#include <zaf/base/non_copyable.h>

namespace zaf::internal {

template<typename C, typename T>
class WrapIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::make_signed_t<std::size_t>;
    using pointer = T*;
    using reference = T&;

public:
    explicit WrapIterator(C::const_iterator inner) : inner_(std::move(inner)) {

    }

    WrapIterator& operator++() {
        inner_++;
        return *this;
    }

    WrapIterator operator++(int) {
        return WrapIterator{ ++inner_ };
    }

    value_type operator*() const {
        return T{ *inner_ };
    }

    bool operator!=(const WrapIterator& other) const {
        return inner_ != other.inner_;
    }

private:
    C::const_iterator inner_;
};


template<typename C, typename T> 
class WrapEnumerator : NonCopyable {
public:
    using Iterator = WrapIterator<C, T>;

public:
    explicit WrapEnumerator(const C& container) : inner_(container) {

    }

    bool IsEmpty() const noexcept {
        return !(inner_.begin() != inner_.end());
    }

    Iterator begin() const {
        return Iterator{ inner_.begin() };
    }

    Iterator end() const {
        return Iterator{ inner_.end() };
    }

private:
    const C& inner_;
};

}