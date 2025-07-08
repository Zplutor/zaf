#pragma once

#include <zaf/rx/observable.h>
#include <zaf/rx/internal/observable_factory.h>
#include <zaf/rx/internal/observable/async_customized_observable.h>
#include <zaf/rx/internal/observable/concat_observable.h>
#include <zaf/rx/internal/observable/customized_observable.h>
#include <zaf/rx/internal/observable/empty_observable.h>
#include <zaf/rx/internal/observable/just_observable.h>
#include <zaf/rx/internal/observable/never_observable.h>
#include <zaf/rx/internal/observable/throw_observable.h>
#include <zaf/rx/subscription.h>

namespace zaf::rx {

/**
Creates an observable that emits no items but terminates normally.
*/
template<typename T = zaf::None>
Observable<T> Empty() {
    auto core = internal::EmptyObservable::Instance();
    return internal::ObservableFactory<T>::CreateObservable(std::move(core));
}


/**
Creates an observable that emits no items and does not terminate.
*/
template<typename T>
Observable<T> Never() {
    auto core = internal::NeverObservable::Instance();
    return internal::ObservableFactory<T>::CreateObservable(std::move(core));
}


template<typename T, typename C>
Observable<T> Concat(const C& container) {
    internal::ObservableList observables;
    for (const auto& each_observable : container) {
        observables.push_back(each_observable.Core());
    }
    auto core = std::make_shared<internal::ConcatObservable>(std::move(observables));
    return internal::ObservableFactory<T>::CreateObservable(std::move(core));
}


template<typename T>
Observable<T> Concat(std::initializer_list<Observable<T>> observables) {
    return Concat<T, std::initializer_list<Observable<T>>>(observables);
}


template<typename T>
Observable<T> Create(std::function<Subscription(Observer<T>)> procedure) {

    auto bridged_procedure = [procedure{ std::move(procedure) }](
        const std::shared_ptr<internal::ObserverCore>& observer_core) {

        auto observer = internal::ObservableFactory<T>::CreateObserver(observer_core);
        auto subscription = procedure(observer);
        return subscription.Core();
    };

    auto core = std::make_shared<internal::CustomizedObservable>(std::move(bridged_procedure));
    return internal::ObservableFactory<T>::CreateObservable(std::move(core));
}


template<typename T>
Observable<T> Create(
    std::shared_ptr<Scheduler> scheduler,
    std::function<void(Observer<T>)> procedure) {

    auto bridged_procedure = [procedure = std::move(procedure)](
        const std::shared_ptr<internal::ObserverCore>& observer_core,
        CancelToken cancel_token) {

        auto observer = internal::ObservableFactory<T>::CreateObserver(observer_core);
        procedure(observer);
    };

    auto core = std::make_shared<internal::AsyncCustomizedObservable>(
        std::move(scheduler),
        std::move(bridged_procedure));

    return internal::ObservableFactory<T>::CreateObservable(std::move(core));
}


template<typename T>
Observable<T> Create(
    std::shared_ptr<Scheduler> scheduler,
    std::function<void(Observer<T>, CancelToken)> procedure) {

    auto bridged_procedure = [procedure = std::move(procedure)](
        const std::shared_ptr<internal::ObserverCore>& observer_core,
        CancelToken cancel_token) {

        auto observer = internal::ObservableFactory<T>::CreateObserver(observer_core);
        procedure(observer, cancel_token);
    };

    auto core = std::make_shared<internal::AsyncCustomizedObservable>(
        std::move(scheduler),
        std::move(bridged_procedure));

    return internal::ObservableFactory<T>::CreateObservable(std::move(core));
}

}