#pragma once

#include <zaf/rx/internal/observable/custom_observable.h>
#include <zaf/rx/internal/observable/just_observable.h>
#include <zaf/rx/internal/observable/never_observable.h>
#include <zaf/rx/internal/observable/observable_core.h>
#include <zaf/rx/internal/observable/throw_observable.h>
#include <zaf/rx/observer_functions.h>

namespace zaf::rx {

template<
    template<typename> typename OBSERVABLE,
    template<typename> typename OBSERVER,
    template<typename> typename SUBSCRIBER,
    typename T
>
class ObservableBase {
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

    static OBSERVABLE<T> Create(std::function<void(SUBSCRIBER<T>)> producer) {

        auto bridged_producer = [producer = std::move(producer)](
            std::shared_ptr<internal::ObserverCore> observer_core,
            std::shared_ptr<Disposable> subscription) {

            SUBSCRIBER<T> subscriber{ std::move(observer_core), std::move(subscription) };
            producer(std::move(subscriber));
        };

        auto core = std::make_shared<internal::CustomObservable>(std::move(bridged_producer));
        return OBSERVABLE<T>{ std::move(core) };
    }

    static OBSERVABLE<T> CreateOn(
        std::shared_ptr<Scheduler> scheduler, 
        std::function<void(SUBSCRIBER<T>)> producer) {

        return Create(std::move(producer)).SubscribeOn(std::move(scheduler));
    }

public:
    [[nodiscard]]
    std::shared_ptr<Disposable> Subscribe(const OBSERVER<T>& observer) {
        auto observer_core = internal::InsiderT<OBSERVER<T>>::GetCore(observer);
        return core_->Subscribe(internal::ObserverShim::FromShared(std::move(observer_core)));
    }

    [[nodiscard]]
    std::shared_ptr<Disposable> Subscribe() {
        return Subscribe(OBSERVER<T>::Create());
    }

    OBSERVABLE<T> SubscribeOn(std::shared_ptr<Scheduler> scheduler) {
        return OBSERVABLE<T>{ core_->SubscribeOn(std::move(scheduler)) };
    }

    OBSERVABLE<T> ObserveOn(std::shared_ptr<Scheduler> scheduler) {
        return OBSERVABLE<T>{ core_->ObserveOn(std::move(scheduler)) };
    }

    OBSERVABLE<T> Do(const OBSERVER<T>& observer) {
        auto observer_core = internal::InsiderT<OBSERVER<T>>::GetCore(observer);
        return OBSERVABLE<T>{ core_->Do(std::move(observer_core)) };
    }

    OBSERVABLE<T> DoOnError(rx::OnError on_error) {
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
    OBSERVABLE<T> DoOnTerminate(Closure work) {
        const auto& core = static_cast<OBSERVABLE<T>*>(this)->Core();
        return OBSERVABLE<T>{ core->DoOnTerminate(std::move(work)) };
    }

    OBSERVABLE<T> DoAfterTerminate(Closure work) {
        const auto& core = static_cast<OBSERVABLE<T>*>(this)->Core();
        return OBSERVABLE<T>{ core->DoAfterTerminate(std::move(work)) };
    }

    OBSERVABLE<T> Finally(Closure work) {
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

protected:
    explicit ObservableBase(std::shared_ptr<internal::ObservableCore> core) noexcept :
        core_(std::move(core)) {

    }

    const std::shared_ptr<internal::ObservableCore>& Core() const noexcept {
        return core_;
    }

private:
    std::shared_ptr<internal::ObservableCore> core_;
};

}
