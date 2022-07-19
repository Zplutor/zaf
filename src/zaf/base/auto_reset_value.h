#pragma once

#include <zaf/base/non_copyable.h>

namespace zaf {

template<typename T>
class AutoResetValue : NonCopyable {
public:
    class Guard : NonCopyable {
    public:
        Guard(T& guarded_value) : 
            guarded_value_(&guarded_value), 
            original_value_(guarded_value) {

        }

        Guard(Guard&& rhs) : 
            guarded_value_(rhs.guarded_value_), 
            original_value_(rhs.original_value_) {

            rhs.guarded_value_ = nullptr;
        }

        ~Guard() {
            if (guarded_value_) {
                *guarded_value_ = original_value_;
            }
        }

    private:
        T* guarded_value_{};
        T original_value_{};
    };

public:
    AutoResetValue() = default;

    AutoResetValue(const T& value) : value_(value) {

    }

    [[nodiscard]]
    Guard BeginSet(const T& value) {
        Guard guard(value_);
        value_ = value;
        return guard;
    }

    const T& Value() const {
        return value_;
    }

private:
    T value_{};
};

}