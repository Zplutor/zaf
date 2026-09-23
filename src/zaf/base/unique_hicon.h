#pragma once

/**
@file
    Defines the `zaf::UniqueHICON` class.
*/

#include <Windows.h>
#include <zaf/base/non_copyable.h>

namespace zaf {

/**
A wrapper of the HICON that automatically destroys the icon when it is destructed.
*/
class UniqueHICON : NonCopyable {
public:
    /**
    Constructs an instance with null icon.
    */
    UniqueHICON() noexcept = default;

    /**
    Constructs an instance with the specified icon.
    */
    explicit UniqueHICON(HICON value) noexcept : value_(value) {

    }

    /**
    Destructs the instance by destroying the managed icon if it is valid.
    */
    ~UniqueHICON() {
        Reset();
    }
    
    /**
    Constructs an instance by moving from another instance.
    */
    UniqueHICON(UniqueHICON&& other) noexcept : value_(other.value_) {
        other.value_ = nullptr;
    }

    /**
    Assigns the current instance by moving from another instance.

    @details
        The previous managed icon will be destroyed if it is valid.
    */
    UniqueHICON& operator=(UniqueHICON&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        Reset(other.value_);
        other.value_ = nullptr;
        return *this;
    }

    /**
    Gets the managed icon value.
    */
    HICON Value() const noexcept {
        return value_;
    }

    /**
    Determines whether the managed icon is valid, which is not null.
    */
    bool IsValid() const noexcept {
        return (value_ != nullptr);
    }

    /**
    Converts the instance to a boolean value, indicating whether the managed icon is valid.
    */
    explicit operator bool() const noexcept {
        return IsValid();
    }

    /**
    Resets the managed icon to a new value.

    @param new_value
        The new icon value.

    @details
        The previous managed icon will be destroyed if it is valid.
    */
    void Reset(HICON new_value = nullptr) noexcept {
        if (IsValid()) {
            ::DestroyIcon(value_);
        }
        value_ = new_value;
    }

private:
    HICON value_{};
};

}