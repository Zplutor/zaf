#pragma once

#include <zaf/base/non_copyable.h>

namespace zaf {

template<typename T>
class AutoReset : NonCopyable {
public:
    explicit AutoReset(T& target) : target_(&target), reset_value_(target) {

    }

    ~AutoReset() {
        Reset();
    }

    AutoReset(AutoReset&& other) : target_(other.target_), reset_value_(other.reset_value_) {
        other.target_ = nullptr;
    }

    AutoReset& operator=(AutoReset&& other) {
        if (this != &other) {
            Reset();
            target_ = other.target_;
            reset_value_ = other.reset_value_;
            other.target_ = nullptr;
        }
        return *this;
    }

private:
    void Reset() {
        if (target_) {
            *target_ = reset_value_;
        }
    }

private:
    T* target_{};
    T reset_value_{};
};


template<typename T>
AutoReset<T> MakeAutoReset(T& target, T new_value) {
    AutoReset<T> auto_reset{ target };
    target = std::move(new_value);
    return auto_reset;
}


template<typename T>
AutoReset<T> MakeAutoReset(T& target) {
    return AutoReset<T>{ target };
}

}