#pragma once

#include <zaf/rx/internal/inner_observer.h>
#include <zaf/rx/observer_functions.h>

namespace zaf::rx {

template<typename T>
using OnSuccess = OnNext<T>;

template<typename T>
class SingleObserver {
public:
    static SingleObserver Create(OnSuccess<T> on_success) {
        return Create(std::move(on_success), nullptr);
    }

    static SingleObserver Create(OnSuccess<T> on_success, OnError on_error) {

        auto on_success_bridge = [inner = std::move(on_success)](const std::any& value) {
            if (inner) {
                inner(std::any_cast<T>(value));
            }
        };

        return SingleObserver{ 
            internal::InnerObserver::Create(
                std::move(on_success_bridge), 
                std::move(on_error), 
                nullptr)
        };
    }

public:
    explicit SingleObserver(std::shared_ptr<internal::InnerObserver> core) :
        core_(std::move(core)) {
    }

    void OnSuccess(const T& value) {
        core_->OnNext(value);
        core_->OnCompleted();
    }

    void OnError(const std::exception_ptr& error) {
        core_->OnError(error);
    }

    const std::shared_ptr<internal::InnerObserver>& Core() const noexcept {
        return core_;
    }

private:
    std::shared_ptr<internal::InnerObserver> core_;
};

}