#pragma once

#include <stdexcept>
#include <type_traits>

namespace zaf {

class bad_optional_access : public std::logic_error {
public:
    bad_optional_access() : logic_error(std::string()) { }
};

template<typename T>
class optional {
public:
    optional() : has_value_(false) { }

    optional(const optional& other) {
        copy_from_other(other);
    }

    optional(optional&& other) {
        move_from_other(std::forward<optional>(other));
    }

    optional(const T& value) {
        construct_with_value(value);
    }

    optional(T&& value) {
        construct_with_value(std::forward<T>(value));
    }

    ~optional() {
        reset();
    }

    optional& operator=(const T& value) {
        reset();
        construct_with_value(value);
        return *this;
    }

    optional& operator=(T&& value) {
        reset();
        construct_with_value(std::forward<T>(value));
        return *this;
    }

    optional& operator=(const optional& other) {
        reset();
        copy_from_other(other);
        return *this;
    }

    optional& operator=(optional&& other) {
        reset();
        move_from_other(std::forward<optional>(other));
        return *this;
    }

    explicit operator bool() const {
        return has_value();
    }

    bool has_value() const {
        return has_value_;
    }

    const T* operator->() const {
        return get_pointer();
    }

    T* operator->() {
        return get_pointer();
    }

    const T& operator*() const {
        return *get_pointer();
    }

    T& operator*() {
        return *get_pointer();
    }

    T& value() {
        return const_cast<T&>(static_cast<const optional*>(this)->value());
    }

    const T& value() const {
        if (has_value()) {
            return *get_pointer();
        }
        else {
            throw bad_optional_access();
        }
    }

    void reset() {
        if (has_value_) {
            destruct();
            has_value_ = false;
        }
    }

private:
    void copy_from_other(const optional& other) {
        has_value_ = other.has_value();
        if (has_value_) {
            construct(*other);
        }
    }

    void move_from_other(optional&& other) {
        has_value_ = other.has_value();
        if (has_value_) {
            construct(std::move(*other));
            other.reset();
        }
    }

    template<typename U>
    void construct_with_value(U&& value) {
        has_value_ = true;
        construct(std::forward<U>(value));
    }

    template<typename U>
    void construct(U&& value) {
        new (get_pointer()) T(std::forward<U>(value));
    }

    void destruct() {
        get_pointer()->~T();
    }

    T* get_pointer() {
        auto pointer = static_cast<const optional*>(this)->get_pointer();
        return const_cast<T*>(pointer);
    }

    const T* get_pointer() const {
        return reinterpret_cast<const T*>(&data_);
    }

private:
    std::aligned_union_t<sizeof(T), T> data_;
    bool has_value_;
};

}