#pragma once

#include <memory>
#include <zaf/rx/internal/observable/observable_core.h>
#include <zaf/rx/observer.h>
#include <zaf/rx/observer_functions.h>
#include <zaf/rx/subscription.h>

namespace zaf {

class Scheduler;

template<typename T>
class Observable {
public:
    explicit Observable(std::shared_ptr<rx::internal::ObservableCore> core) noexcept :
        core_(std::move(core)) { }

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
        return Subscription{ core_->Subscribe(observer.Core()) };
    }

    Observable SubscribeOn(std::shared_ptr<Scheduler> scheduler) {
        return Observable{ core_->SubscribeOn(std::move(scheduler)) };
    }

    Observable ObserveOn(std::shared_ptr<Scheduler> scheduler) {
        return Observable{ core_->ObserveOn(std::move(scheduler)) };
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
        return Observable{ core_->Do(observer.Core()) };
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
        return Observable{ core_->DoOnTerminated(std::move(action)) };
    }

    Observable Catch(std::function<Observable<T>(const std::exception_ptr&)> handler) {
        return Observable{ 
            core_->Catch([handle = std::move(handler)](const std::exception_ptr& error) {
                return handle(error).Core();
            })
        };
    }

    Observable Finally(Work work) {
        return Observable{ core_->Finally(std::move(work)) };
    }

    template<typename K>
    Observable<K> Map(std::function<K(const T&)> mapper) {
        return Observable<K>{
            core_->Map([map = std::move(mapper)](const std::any& value) {
                return map(std::any_cast<T>(value));
            })
        };
    }

    template<typename K>
    Observable<K> FlatMap(std::function<Observable<K>(const T&)> mapper) {
        return Observable<K>{
            core_->FlatMap([map = std::move(mapper)](const std::any& value) {
                return map(std::any_cast<T>(value)).Core();
            })
        };
    }

    const std::shared_ptr<rx::internal::ObservableCore>& Core() const {
        return core_;
    }

private:
    std::shared_ptr<rx::internal::ObservableCore> core_;
};

}