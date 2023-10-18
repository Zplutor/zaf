#pragma once

#include <memory>
#include <zaf/rx/internal/inner_observer.h>

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
            if (on_next) {
                on_next(std::any_cast<T>(value));
            }
        };

        return Observer(internal::InnerObserver::Create(
            std::move(bridged_on_next), 
            std::move(on_error), 
            std::move(on_completed)));
    }

public:
    explicit Observer(std::shared_ptr<internal::InnerObserver> inner) : 
        inner_(std::move(inner)) { }

    void OnNext(const T& value) const {
        inner_->OnNext(value);
    }

    void OnError(const Error& error) const {
        inner_->OnError(error);
    }

    void OnCompleted() const {
        inner_->OnCompleted();
    }

    const std::shared_ptr<internal::InnerObserver>& Inner() const {
        return inner_;
    }

private:
    std::shared_ptr<internal::InnerObserver> inner_;
};

}