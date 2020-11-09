#pragma once

#include <memory>
#include <zaf/rx/internal/observable_implementation.h>
#include <zaf/rx/observer.h>
#include <zaf/rx/observer_functions.h>
#include <zaf/rx/subscription.h>

namespace zaf {

template<typename T>
class Observable {
public:
    explicit Observable(std::shared_ptr<internal::ObservableImplementation> implementation) : 
        implementation_(std::move(implementation)) { }

    Subscription Subscribe(OnNext<T> on_next) {
        return Subscribe(std::move(on_next), nullptr, nullptr);
    }

    Subscription Subscribe(OnNext<T> on_next, OnError on_error) {
        return Subscribe(std::move(on_next), std::move(on_error), nullptr);
    }

    Subscription Subscribe(OnNext<T> on_next, OnCompleted on_completed) {
        return Subscribe(std::move(on_next), nullptr, std::move(on_completed));
    }

    Subscription Subscribe(OnNext<T> on_next, OnError on_error, OnCompleted on_completed) {
        return Subscribe(Observer<T>::Create(
            std::move(on_next), 
            std::move(on_error), 
            std::move(on_completed)));
    }

    Subscription Subscribe(const Observer<T>& observer) {
        return Subscription(implementation_->Subscribe(observer.GetImplementation()));
    }

    const std::shared_ptr<internal::ObservableImplementation>& GetImplementation() const {
        return implementation_;
    }

private:
    std::shared_ptr<internal::ObservableImplementation> implementation_;
};

}