#pragma once

#include <memory>
#include <zaf/rx/internal/observable/inner_observable.h>
#include <zaf/rx/observer.h>
#include <zaf/rx/observer_functions.h>
#include <zaf/rx/subscription.h>

namespace zaf {

class Scheduler;

template<typename T>
class Observable {
public:
    explicit Observable(std::shared_ptr<internal::InnerObservable> inner) : 
        inner_(std::move(inner)) { }

    Subscription Subscribe() {
        return Subscribe(nullptr, nullptr, nullptr);
    }

    [[nodiscard]]
    Subscription Subscribe(OnNext<T> on_next) {
        return Subscribe(std::move(on_next), nullptr, nullptr);
    }

    [[nodiscard]]
    Subscription Subscribe(OnNext<T> on_next, OnError on_error) {
        return Subscribe(std::move(on_next), std::move(on_error), nullptr);
    }

    [[nodiscard]]
    Subscription Subscribe(OnNext<T> on_next, OnCompleted on_completed) {
        return Subscribe(std::move(on_next), nullptr, std::move(on_completed));
    }

    [[nodiscard]]
    Subscription Subscribe(OnNext<T> on_next, OnError on_error, OnCompleted on_completed) {
        return Subscribe(Observer<T>::Create(
            std::move(on_next), 
            std::move(on_error), 
            std::move(on_completed)));
    }

    [[nodiscard]]
    Subscription Subscribe(const Observer<T>& observer) {
        return Subscription{ inner_->Subscribe(observer.Inner()) };
    }

    Observable SubscribeOn(std::shared_ptr<Scheduler> scheculer) {
        return Observable{ inner_->SubscribeOn(std::move(scheculer)) };
    }

    Observable ObserveOn(std::shared_ptr<Scheduler> scheduler) {
        return Observable{ inner_->ObserveOn(std::move(scheduler)) };
    }

    Observable Do(OnNext<T> on_next) {
        return Do(std::move(on_next), nullptr, nullptr);
    }

    Observable Do(OnNext<T> on_next, OnCompleted on_completed) {
        return Do(std::move(on_next), nullptr, std::move(on_completed));
    }

    Observable Do(OnNext<T> on_next, OnError on_error) {
        return Do(std::move(on_next), std::move(on_error), nullptr);
    }

    Observable Do(OnNext<T> on_next, OnError on_error, OnCompleted on_completed) {
        return Do(Observer<T>::Create(
            std::move(on_next),
            std::move(on_error),
            std::move(on_completed)));
    }

    Observable Do(const Observer<T>& observer) {
        return Observable{ inner_->Do(observer.Inner()) };
    }

    Observable Finally(Work work) {
        return Observable{ inner_->Finally(std::move(work)) };
    }

    const std::shared_ptr<internal::InnerObservable>& Inner() const {
        return inner_;
    }

private:
    std::shared_ptr<internal::InnerObservable> inner_;
};

}