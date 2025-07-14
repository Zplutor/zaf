#pragma once

#include <zaf/rx/internal/observable/custom_observable.h>
#include <zaf/rx/internal/observable/just_observable.h>
#include <zaf/rx/internal/observable/never_observable.h>
#include <zaf/rx/internal/observable/observable_core.h>
#include <zaf/rx/internal/observable/throw_observable.h>
#include <zaf/rx/observer_functions.h>
#include <zaf/rx/subscription.h>

namespace zaf::rx {

template<
    template<typename> typename OBSERVABLE,
    template<typename> typename OBSERVER,
    template<typename> typename SUBSCRIBER,
    typename T
>
class BaseObservable {
public:
    static OBSERVABLE<T> Just(T value) {
        auto core = std::make_shared<internal::JustObservable>(std::any{ std::move(value) });
        return OBSERVABLE<T>{ std::move(core) };
    }

    /**
    Creates an observable that emits no items and terminates with an error.
    */
    static OBSERVABLE<T> Throw(std::exception_ptr error) {
        auto core = std::make_shared<internal::ThrowObservable>(std::move(error));
        return OBSERVABLE<T>{ std::move(core) };
    }

    /**
    Creates an observable that emits no items and terminates with an error.
    */
    template<typename E>
    static OBSERVABLE<T> Throw(E&& error) {
        return Throw(std::make_exception_ptr(std::forward<E>(error)));
    }

    /**
    Creates an observable that emits no items and does not terminate.
    */
    static OBSERVABLE<T> Never() {
        return OBSERVABLE<T>{ internal::NeverObservable::Instance() };
    }

    static OBSERVABLE<T> Create(std::function<void(SUBSCRIBER<T> subscriber)> producer) {

        auto bridged_producer = [producer = std::move(producer)](
            std::shared_ptr<internal::ObserverCore> observer_core,
            std::shared_ptr<internal::SubscriptionCore> subscription_core) {

            auto subscriber = SUBSCRIBER<T>{ 
                std::move(observer_core),
                std::move(subscription_core) 
            };
            producer(std::move(subscriber));
        };

        auto core = std::make_shared<internal::CustomObservable>(std::move(bridged_producer));
        return OBSERVABLE<T>{ std::move(core) };
    }

public:
    [[nodiscard]]
    Subscription Subscribe(const OBSERVER<T>& observer) {
        return Subscription{ core_->Subscribe(observer.Core()) };
    }

    [[nodiscard]]
    Subscription Subscribe() {
        return Subscribe(OBSERVER<T>::Create());
    }

    OBSERVABLE<T> SubscribeOn(std::shared_ptr<Scheduler> scheduler) {
        return OBSERVABLE<T>{ core_->SubscribeOn(std::move(scheduler)) };
    }

    OBSERVABLE<T> ObserveOn(std::shared_ptr<Scheduler> scheduler) {
        return OBSERVABLE<T>{ core_->ObserveOn(std::move(scheduler)) };
    }

    OBSERVABLE<T> Do(const OBSERVER<T>& observer) {
        return OBSERVABLE<T>{ core_->Do(observer.Core()) };
    }

    OBSERVABLE<T> DoOnError(zaf::OnError on_error) {
        return static_cast<OBSERVABLE<T>*>(this)->Do(OBSERVER<T>::Create(std::move(on_error)));
    }

    template<typename E>
    OBSERVABLE<T> DoOnError(std::function<void(const E&)> handler) {

        auto on_error = [handler = std::move(handler)](const std::exception_ptr& exception) {
            try {
                std::rethrow_exception(exception);
            }
            catch (const E& error) {
                handler(error);
            }
            catch (...) {

            }
        };
        return static_cast<OBSERVABLE<T>*>(this)->Do(OBSERVER<T>::Create(std::move(on_error)));
    }

    OBSERVABLE<T> Catch(std::function<OBSERVABLE<T>(const std::exception_ptr&)> handler) {

        const auto& core = static_cast<OBSERVABLE<T>*>(this)->Core();
        auto new_core = core->Catch(
            [handler = std::move(handler)](const std::exception_ptr& error) {
            return handler(error).Core();
        });
        return OBSERVABLE<T>{ std::move(new_core) };
    }

    /**
    Returns an observable that invokes an action when the current observable is terminated, either
    by completion or error.

    @param action
        The action to be invoked upon termination of the current observable.

    @return
        A new observable.
    */
    OBSERVABLE<T> DoOnTerminate(Work work) {
        const auto& core = static_cast<OBSERVABLE<T>*>(this)->Core();
        return OBSERVABLE<T>{ core->DoOnTerminate(std::move(work)) };
    }

    OBSERVABLE<T> DoAfterTerminate(Work work) {
        const auto& core = static_cast<OBSERVABLE<T>*>(this)->Core();
        return OBSERVABLE<T>{ core->DoAfterTerminate(std::move(work)) };
    }

    OBSERVABLE<T> Finally(Work work) {
        return OBSERVABLE<T>{ core_->Finally(std::move(work)) };
    }

    template<typename K>
    OBSERVABLE<K> Map(std::function<K(const T&)> mapper) {
        auto new_core = core_->Map([mapper = std::move(mapper)](const std::any& value) {
            return mapper(std::any_cast<T>(value));
        });
        return OBSERVABLE<K>{ std::move(new_core) };
    }

    template<typename K>
    OBSERVABLE<K> FlatMap(std::function<OBSERVABLE<K>(const T&)> mapper) {

        const auto& core = static_cast<const OBSERVABLE<T>*>(this)->Core();
        auto new_core = core->FlatMap([mapper = std::move(mapper)](const std::any& value) {
            return mapper(std::any_cast<T>(value)).Core();
        });
        return OBSERVABLE<K>{ std::move(new_core) };
    }

    const std::shared_ptr<internal::ObservableCore>& Core() const noexcept {
        return core_;
    }

protected:
    explicit BaseObservable(std::shared_ptr<internal::ObservableCore> core) noexcept :
        core_(std::move(core)) {

    }

private:
    std::shared_ptr<internal::ObservableCore> core_;
};

}
