#pragma once

#include <zaf/rx/base_observable.h>
#include <zaf/rx/continuous_observer.h>
#include <zaf/rx/internal/observable/never_observable.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/subscription.h>

namespace zaf::rx::internal {
template<typename T>
class ContinuousFactory;
}

namespace zaf::rx {

template<typename T>
class Continuous : public BaseObservable<Continuous, T> {
public:
    static Continuous Never() {
        return Continuous{ internal::NeverObservable::Instance() };
    }

public:
    [[nodiscard]]
    Subscription Subscribe() {
        return this->Subscribe(nullptr);
    }

    [[nodiscard]] 
    Subscription Subscribe(OnNext<T> on_next) {
        auto observer = ContinuousObserver<T>::Create(std::move(on_next));
        return Subscription{ this->Core()->Subscribe(observer.Core()) };
    }

    Continuous Do(OnNext<T> on_next) {
        auto observer = ContinuousObserver<T>::Create(std::move(on_next));
        return this->Core()->Do(observer.Core());
    }

    operator Observable<T>() const noexcept {
        return Observable<T>{ this->Core() };
    }

private:
    friend class BaseObservable<Continuous, T>;

    template<template<typename> typename OBSERVABLE, typename K>
    friend class BaseObservable;

    friend class internal::ContinuousFactory<T>;

    explicit Continuous(std::shared_ptr<internal::ObservableCore> core) noexcept :
        BaseObservable<Continuous, T>(std::move(core)) {

    }
};

}