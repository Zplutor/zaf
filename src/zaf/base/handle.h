#pragma once

#include <Windows.h>
#include <zaf/base/error/contract_error.h>

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

    explicit operator bool() const {
        return IsValid();
    }

    HANDLE Value() const {
        return value_;
    }

    HANDLE operator*() const {
        return value_;
    }

    HANDLE* Pointer() {
        ZAF_EXPECT(!IsValid());
        return &value_;
    }

private:
    HANDLE value_{};
};

}