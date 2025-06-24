#pragma once

#include <zaf/rx/internal/observable/inner_observable.h>
#include <zaf/rx/internal/observable/just_observable.h>
#include <zaf/rx/internal/observable/never_observable.h>
#include <zaf/rx/internal/observable/throw_observable.h>
#include <zaf/rx/single_observer.h>
#include <zaf/rx/subscription.h>

namespace zaf::rx {

template<typename T>
class Single {
public:
    static Single Just(T value) {
        return Single{ std::make_shared<internal::JustObservable>(std::any{ std::move(value) }) };
    }

    static Single Throw(std::exception_ptr error) {
        return Single{ std::make_shared<internal::ThrowObservable>(std::move(error)) };
    }

    template<typename E>
    static Single Throw(E&& error) {
        return Throw(std::make_exception_ptr(std::forward<E>(error)));
    }

    static Single Never() {
        return Single{ internal::NeverObservable::Instance() };
    }

public:
    [[nodiscard]]
    Subscription Subscribe(OnSuccess<T> on_success) {
        return Subscribe(std::move(on_success), nullptr);
    }

    [[nodiscard]]
    Subscription Subscribe(OnSuccess<T> on_success, OnError on_error) {
        auto observer = SingleObserver<T>::Create(std::move(on_success), std::move(on_error));
        return Subscription{ core_->Subscribe(observer.Core()) };
    }

    const std::shared_ptr<internal::InnerObservable>& Core() const noexcept {
        return core_;
    }

private:
    explicit Single(std::shared_ptr<internal::InnerObservable> core) noexcept :
        core_(std::move(core)) {

    }

private:
    std::shared_ptr<internal::InnerObservable> core_;
};

}