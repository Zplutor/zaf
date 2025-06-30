#pragma once

#include <memory>
#include <zaf/rx/base_observable.h>
#include <zaf/rx/internal/observable/observable_core.h>
#include <zaf/rx/observer.h>
#include <zaf/rx/observer_functions.h>
#include <zaf/rx/subscription.h>

namespace zaf {

class Scheduler;

template<typename T>
class Observable : public rx::BaseObservable<Observable, T> {
public:
    explicit Observable(std::shared_ptr<rx::internal::ObservableCore> core) noexcept :
        rx::BaseObservable<Observable, T>(std::move(core)) { }

    [[nodiscard]]
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
        return Subscription{ this->Core()->Subscribe(observer.Core())};
    }

    Observable SubscribeOn(std::shared_ptr<Scheduler> scheduler) {
        return Observable{ this->Core()->SubscribeOn(std::move(scheduler)) };
    }

    Observable ObserveOn(std::shared_ptr<Scheduler> scheduler) {
        return Observable{ this->Core()->ObserveOn(std::move(scheduler)) };
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
        return Observable{ this->Core()->Do(observer.Core()) };
    }

    Observable DoOnError(OnError on_error) {
        return Do(nullptr, std::move(on_error), nullptr);
    }

    template<typename E>
    Observable DoOnError(std::function<void(const E&)> handler) {
        return Do(
            nullptr, 
            [handle = std::move(handler)](const std::exception_ptr& exception) {
                try {
                    std::rethrow_exception(exception);
                }
                catch (const E& error) {
                    handle(error);
                }
                catch (...) {

                }
            },
            nullptr);
    }

    Observable DoOnCompleted(OnCompleted on_completed) {
        return Do(nullptr, nullptr, std::move(on_completed));
    }

    Observable Catch(std::function<Observable<T>(const std::exception_ptr&)> handler) {
        return Observable{ 
            this->Core()->Catch([handle = std::move(handler)](const std::exception_ptr& error) {
                return handle(error).Core();
            })
        };
    }
};

}