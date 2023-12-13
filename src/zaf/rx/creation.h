#pragma once

#include <zaf/rx/observable.h>
#include <zaf/rx/internal/observable/async_customized_observable.h>
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
template<typename T>
Observable<T> Empty() {
    return Observable<T>{ internal::EmptyObservable::Instance() };
}


/**
Creates an observable that emits no items and does not terminate.
*/
template<typename T>
Observable<T> Never() {
    return Observable<T>{ internal::NeverObservable::Instance() };
}


/**
Creates an observable that emits no items and terminates with an error.
*/
template<typename T>
Observable<T> Throw(zaf::Error error) {
    return Observable<T>{ std::make_shared<internal::ThrowObservable>(std::move(error)) };
}


template<typename T>
Observable<T> Just(const T& value) {
    return Observable<T>(std::make_shared<internal::JustObservable>(std::any{ value }));
}


template<typename T>
Observable<T> Create(std::function<Subscription(Observer<T>)> procedure) {

    auto bridged_procedure = [procedure{ std::move(procedure) }](
        const std::shared_ptr<internal::InnerObserver>& observer) {
    
        auto subscription = procedure(Observer<T>{ observer });
        return subscription.Inner();
    };

    auto customized_observable = 
        std::make_shared<internal::CustomizedObservable>(std::move(bridged_procedure));

    return Observable<T>(std::move(customized_observable));
}


template<typename T>
Observable<T> Create(
    std::shared_ptr<Scheduler> schduler,
    std::function<void(Observer<T>)> procedure) {

    auto bridged_procedure = [procedure = std::move(procedure)](
        const std::shared_ptr<internal::InnerObserver>& observer,
        CancelToken cancel_token) {

        procedure(Observer<T>{ observer });
    };

    auto observable = std::make_shared<internal::AsyncCustomizedObservable>(
        std::move(schduler),
        std::move(bridged_procedure));

    return Observable<T>(std::move(observable));
}


template<typename T>
Observable<T> Create(
    std::shared_ptr<Scheduler> schduler,
    std::function<void(Observer<T>, CancelToken)> procedure) {

    auto bridged_procedure = [procedure = std::move(procedure)](
        const std::shared_ptr<internal::InnerObserver>& observer, 
        CancelToken cancel_token) {

        procedure(Observer<T>{ observer }, cancel_token);
    };

    auto observable = std::make_shared<internal::AsyncCustomizedObservable>(
        std::move(schduler),
        std::move(bridged_procedure));

    return Observable<T>(std::move(observable));
}

}