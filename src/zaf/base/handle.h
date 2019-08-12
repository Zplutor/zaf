#pragma once

#include <Windows.h>

namespace zaf {

class Handle {
public:
    Handle() {}
    explicit Handle(HANDLE value) : value_(value) { }

    ~Handle() {
        if (value_ != nullptr && value_ != INVALID_HANDLE_VALUE) {
            CloseHandle(value_);
        }
    }

    Handle(Handle&& other) : value_(other.value_) {
        other.value_ = nullptr;
    }

    Handle& operator=(Handle&& other) {
        value_ = other.value_;
        other.value_ = nullptr;
        return *this;
    }

    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;

    HANDLE GetValue() const {
        return value_;
    }

private:
    HANDLE value_{};
};

}