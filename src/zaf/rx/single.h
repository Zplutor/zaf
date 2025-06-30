#pragma once

#include <zaf/rx/base_observable.h>
#include <zaf/rx/internal/observable/just_observable.h>
#include <zaf/rx/internal/observable/never_observable.h>
#include <zaf/rx/internal/observable/throw_observable.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/single_observer.h>
#include <zaf/rx/subscription.h>

namespace zaf::rx::internal {
template<typename T>
class SingleFactory;
}

namespace zaf::rx {

template<typename T>
class Single : public BaseObservable<Single, T> {
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
    Subscription Subscribe() {
        return Subscribe(nullptr, nullptr);
    }

    [[nodiscard]]
    Subscription Subscribe(OnSuccess<T> on_success) {
        return Subscribe(std::move(on_success), nullptr);
    }

    [[nodiscard]]
    Subscription Subscribe(OnSuccess<T> on_success, OnError on_error) {
        auto observer = SingleObserver<T>::Create(std::move(on_success), std::move(on_error));
        return Subscription{ this->Core()->Subscribe(observer.Core())};
    }

    Single Do(OnSuccess<T> on_success) {
        return Do(SingleObserver<T>::Create(std::move(on_success), nullptr));
    }

    Single Do(OnSuccess<T> on_success, OnError on_error) {
        return Do(SingleObserver<T>::Create(std::move(on_success), std::move(on_error)));
    }

    Single Do(const SingleObserver<T>& observer) {
        return Single{ this->Core()->Do(observer.Core()) };
    }

    Single DoOnError(OnError on_error) {
        return Do(nullptr, std::move(on_error));
    }

    template<typename E>
    Single DoOnError(std::function<void(const E&)> handler) {
        return Do(
            nullptr, 
            [handler = std::move(handler)](const std::exception_ptr& exception) {
                try {
                    std::rethrow_exception(exception);
                }
                catch (const E& error) {
                    handler(error);
                }
                catch (...) {

                }
            });
    }

    /**
    Converts a single to an observable implicitly.

    @details
        We define a conversion operator in `Single<>` rather than defining a constructor in
        `Observable<>` to avoid `Observable<>` depending on `Single<>`, as `Single<>` is a
        specialized type of `Observable<>`.
    */
    operator Observable<T>() const noexcept {
        return Observable<T>{ this->Core() };
    }

private:
    template<typename K>
    friend class Single;

    friend class BaseObservable<Single, T>;

    template<template<typename> typename OBSERVABLE, typename K>
    friend class BaseObservable;

    friend class zaf::rx::internal::SingleFactory<T>;

    explicit Single(std::shared_ptr<internal::ObservableCore> core) noexcept : 
        BaseObservable<Single, T>(std::move(core)) {

    }
};

}