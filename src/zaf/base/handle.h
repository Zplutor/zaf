#pragma once

#include <Windows.h>

namespace zaf {

class Handle {
public:
    Handle() = default;
    explicit Handle(HANDLE value) : value_(value) { }

    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;

    Handle(Handle&& other) : value_(other.value_) {
        other.value_ = nullptr;
    }

    Handle& operator=(Handle&& other) {
        Reset(other.value_);
        other.value_ = nullptr;
        return *this;
    }

    Handle& operator=(HANDLE new_value) {
        Reset(new_value);
        return *this;
    }

    ~Handle() {
        Reset();
    }

    void Reset(HANDLE new_value = nullptr) {
        if (IsValid()) {
            CloseHandle(value_);
        }
        value_ = new_value;
    }

    bool IsValid() const {
        return (value_ != nullptr) && (value_ != INVALID_HANDLE_VALUE);
    }

    HANDLE GetValue() const {
        return value_;
    }

    explicit operator bool() const {
        return IsValid();
    }

private:
    HANDLE value_{};
};

}