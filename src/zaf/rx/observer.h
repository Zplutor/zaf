#pragma once

#include <memory>
#include <zaf/rx/internal/observer_implementation.h>

namespace zaf {

template<typename T>
class Observer {
public:
    static Observer Create(OnNext<T> on_next, OnCompleted on_completed = nullptr) {
        return Create(std::move(on_next), nullptr, std::move(on_completed));
    }

    static Observer Create(
        OnNext<T> on_next, 
        OnError on_error = nullptr, 
        OnCompleted on_completed = nullptr) {

        auto bridged_on_next = [on_next](const std::any& value) {
            on_next(std::any_cast<T>(value));
        };

        return Observer(internal::ObserverImplementation::Create(
            std::move(bridged_on_next), 
            std::move(on_error), 
            std::move(on_completed)));
    }

public:
    explicit Observer(std::shared_ptr<internal::ObserverImplementation> implementation) : 
        implementation_(std::move(implementation)) { }

    void OnNext(const T& value) {
        implementation_->OnNext(value);
    }

    void OnError(const Error& error) {
        implementation_->OnError(error);
    }

    void OnCompleted() {
        implementation_->OnCompleted();
    }

    const std::shared_ptr<internal::ObserverImplementation>& GetImplementation() const {
        return implementation_;
    }

private:
    std::shared_ptr<internal::ObserverImplementation> implementation_;
};

}