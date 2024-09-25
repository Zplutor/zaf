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

    /**
    Returns an Observable that invokes an action when the current Observable is terminated, either 
    by completion or error.

    @param action
        The action to be invoked upon termination of the current observable.

    @return
        A new observable.
    */
    Observable DoOnTerminated(Work action) {
        return Observable{ inner_->DoOnTerminated(std::move(action)) };
    }

    Observable Catch(std::function<Observable<T>(const std::exception_ptr&)> handler) {
        return Observable{ 
            inner_->Catch([handle = std::move(handler)](const std::exception_ptr& error) {
                return handle(error).Inner();
            })
        };
    }

    Observable Finally(Work work) {
        return Observable{ inner_->Finally(std::move(work)) };
    }

    template<typename K>
    Observable<K> Map(std::function<K(const T&)> mapper) {
        return Observable<K>{
            inner_->Map([map = std::move(mapper)](const std::any& value) {
                return map(std::any_cast<T>(value));
            })
        };
    }

    template<typename K>
    Observable<K> FlatMap(std::function<Observable<K>(const T&)> mapper) {
        return Observable<K>{
            inner_->FlatMap([map = std::move(mapper)](const std::any& value) {
                return map(std::any_cast<T>(value)).Inner();
            })
        };
    }

    const std::shared_ptr<internal::InnerObservable>& Inner() const {
        return inner_;
    }

private:
    std::shared_ptr<internal::InnerObservable> inner_;
};

}