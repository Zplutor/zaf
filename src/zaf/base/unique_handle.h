#pragma once

/**
@file
    Defines the `zaf::UniqueHANDLE` class.
*/

#include <Windows.h>
#include <zaf/base/non_copyable.h>

namespace zaf {

/**
A wrapper of the HANDLE that automatically closes the handle when it is destructed.
*/
class UniqueHANDLE : NonCopyable {
public:
    /**
    Constructs an instance with null handle.
    */
    UniqueHANDLE() noexcept = default;

    /**
    Constructs an instance with the specified handle.
    */
    explicit UniqueHANDLE(HANDLE value) noexcept : value_(value) { }

    /**
    Destructs the instance by closing the managed handle if it is valid.
    */
    ~UniqueHANDLE() {
        Reset();
    }
    
    /**
    Constructs an instance by moving from another instance.
    */
    UniqueHANDLE(UniqueHANDLE&& other) noexcept : value_(other.value_) {
        other.value_ = nullptr;
    }

    /**
    Assigns the current instance by moving from another instance.

    @details
        The previous managed handle will be closed if it is valid.
    */
    UniqueHANDLE& operator=(UniqueHANDLE&& other) noexcept {
        Reset(other.value_);
        other.value_ = nullptr;
        return *this;
    }

    /**
    Gets the managed handle value.
    */
    HANDLE Value() const noexcept {
        return value_;
    }

    /**
    Determintes whether the managed handle is valid, which is not null and not 
    `INVALID_HANDLE_VALUE`.
    */
    bool IsValid() const noexcept {
        return (value_ != nullptr) && (value_ != INVALID_HANDLE_VALUE);
    }

    /**
    Converts the instance to a boolean value, indicating whether the managed handle is valid.
    */
    explicit operator bool() const noexcept {
        return IsValid();
    }

    /**
    Resets the managed handle to null and returns the address of the managed handle value, which
    can be used in Win32 API functions that return a handle through a pointer.
    
    @details
        The previous managed handle will be closed if it is valid.
    */
    HANDLE* Reset() noexcept {
        Reset(nullptr);
        return &value_;
    }

    /**
    Resets the managed handle to a new value.

    @details
        The previous managed handle will be closed if it is valid.
    */
    void Reset(HANDLE new_value) noexcept {
        if (IsValid()) {
            CloseHandle(value_);
        }
        value_ = new_value;
    }

private:
    HANDLE value_{};
};

}