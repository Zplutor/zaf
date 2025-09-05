#pragma once

/**
@file
    Defines the `zaf::rx::ObservableBase` class template.
*/

#include <zaf/base/error/contract_error.h>
#include <zaf/rx/internal/observable/custom_observable.h>
#include <zaf/rx/internal/observable/just_observable.h>
#include <zaf/rx/internal/observable/never_observable.h>
#include <zaf/rx/internal/observable/observable_core.h>
#include <zaf/rx/internal/observable/throw_observable.h>
#include <zaf/rx/observer_functions.h>

namespace zaf::rx {

/**
A base class template for observable types, providing common observable functionalities.

@tparam OBSERVABLE
    The observable type that derives from this base class template.

@tparam OBSERVER
    The observer type used for subscribing to the observable.

@tparam SUBSCRIBER
    The subscriber type used in the producer function when creating custom observables.

@tparam T
    The type of items emitted by the observable.
*/
template<
    template<typename> typename OBSERVABLE,
    template<typename> typename OBSERVER,
    template<typename> typename SUBSCRIBER,
    typename T
>
class ObservableBase {
public:
    /**
    Creates an observable that emits a single item and completes.

    @param value
        The item to be emitted.

    @return
        An observable that emits the specified item and then completes.

    @throw std::bad_alloc
    */
    static OBSERVABLE<T> Just(T value) {
        auto core = std::make_shared<internal::JustObservable>(std::any{ std::move(value) });
        return OBSERVABLE<T>{ std::move(core) };
    }

    /**
    Creates an observable that emits no items and terminates with an error.

    @param error
        The error to be emitted.

    @pre
        The error is not null.

    @return
        An observable that emits no items and terminates with the specified error.

    @throw zaf::PreconditionError
    @throw std::bad_alloc
    */
    static OBSERVABLE<T> Throw(std::exception_ptr error) {
        auto core = std::make_shared<internal::ThrowObservable>(std::move(error));
        return OBSERVABLE<T>{ std::move(core) };
    }

    /**
    Creates an observable that emits no items and terminates with an error.
    
    @tparam E
        The type of the error.

    @param error
        The error to be emitted, which will be converted to `std::exception_ptr`.

    @return
        An observable that emits no items and terminates with the specified error.

    @throw std::bad_alloc
    */
    template<typename E>
    static OBSERVABLE<T> Throw(E&& error) {
        return Throw(std::make_exception_ptr(std::forward<E>(error)));
    }

    /**
    Creates an observable that emits no items and does not terminate.

    @return
        An observable that emits no items and does not terminate.

    @throw std::bad_alloc
    */
    static OBSERVABLE<T> Never() {
        return OBSERVABLE<T>{ internal::NeverObservable::Instance() };
    }

    /**
    Creates a custom observable with the specified producer function.

    @param producer
        The producer function that emits items. The function has the following signature:
        @code{.cpp}
        void(SUBSCRIBER<T> subscriber);
        @endcode
        If the producer function throws an exception, the observable will terminate with the
        thrown exception as an error.

    @pre 
        The producer function is not null.

    @return
        An observable that emits items produced by the specified producer function.

    @throw zaf::PreconditionError
    @throw std::bad_alloc

    @see zaf::rx::SingleSubscriber<>
    @see zaf::rx::Subscriber<>
    */
    static OBSERVABLE<T> Create(std::function<void(SUBSCRIBER<T>)> producer) {

        ZAF_EXPECT(producer);

        auto bridged_producer = [producer = std::move(producer)](
            std::shared_ptr<internal::ObserverCore> observer_core,
            std::shared_ptr<Disposable> subscription) {

            SUBSCRIBER<T> subscriber{ std::move(observer_core), std::move(subscription) };
            producer(std::move(subscriber));
        };

        auto core = std::make_shared<internal::CustomObservable>(std::move(bridged_producer));
        return OBSERVABLE<T>{ std::move(core) };
    }

    /**
    Creates a custom observable that invokes the specified producer function on the given 
    scheduler.

    @param scheduler
        The scheduler on which to invoke the producer function.

    @param producer
        The producer function that emits items. The function has the following signature:
        @code{.cpp}
        void(SUBSCRIBER<T> subscriber);
        @endcode
        If the producer function throws an exception, the observable will terminate with the
        thrown exception as an error.

    @pre
        - The scheduler is not null.
        - The producer function is not null.

    @return
        An observable that emits items produced by the specified producer function.

    @throw zaf::PreconditionError
    @throw std::bad_alloc

    @details
        This method is equivalent to: 
        @code{.cpp}
        Create(producer).SubscribeOn(scheduler);
        @endcode

    @see zaf::rx::SingleSubscriber<>
    @see zaf::rx::Subscriber<>
    */
    static OBSERVABLE<T> CreateOn(
        std::shared_ptr<Scheduler> scheduler, 
        std::function<void(SUBSCRIBER<T>)> producer) {

        return Create(std::move(producer)).SubscribeOn(std::move(scheduler));
    }

public:
    /**
    Subscribes to the current observable with the specified observer.

    @param observer
        The observer to receive items emitted by the observable.

    @return
        A disposable object that can be used to unsubscribe from the observable. Callers should 
        retain the returned object so that the subscription remains active.

    @post
        The return object is not null.

    @throw ...
        Any exception thrown by the underlying observable implementation when subscribing.
    */
    [[nodiscard]]
    std::shared_ptr<Disposable> Subscribe(const OBSERVER<T>& observer) {
        auto observer_core = internal::InsiderT<OBSERVER<T>>::GetCore(observer);
        return core_->Subscribe(internal::ObserverShim::FromShared(std::move(observer_core)));
    }

    /**
    Subscribes to the current observable without any observer, ignoring all emissions.

    @return
        A disposable object that can be used to unsubscribe from the observable. Callers should 
        retain the returned object so that the subscription remains active.

    @post 
        The return object is not null.

    @throw std::bad_alloc
    @throw ...
        Any exception thrown by the underlying observable implementation when subscribing.
    */
    [[nodiscard]]
    std::shared_ptr<Disposable> Subscribe() {
        return Subscribe(OBSERVER<T>::Create());
    }

    /**
    Creates a new observable that subscribes to the current observable on the specified scheduler.

    @param scheduler
        The scheduler on which to perform the subscription.

    @pre
        The scheduler is not null.

    @return
        An observable that subscribes to the current observable on the specified scheduler.

    @throw zaf::PreconditionError
    @throw std::bad_alloc
    */
    OBSERVABLE<T> SubscribeOn(std::shared_ptr<Scheduler> scheduler) {
        return OBSERVABLE<T>{ core_->SubscribeOn(std::move(scheduler)) };
    }

    /**
    Creates a new observable that emits items on the specified scheduler.

    @param scheduler
        The scheduler on which to emit items.

    @pre
        The scheduler is not null.

    @return
        An observable that emits items on the specified scheduler.

    @throw zaf::PreconditionError
    @throw std::bad_alloc
    */
    OBSERVABLE<T> ObserveOn(std::shared_ptr<Scheduler> scheduler) {
        return OBSERVABLE<T>{ core_->ObserveOn(std::move(scheduler)) };
    }

    /**
    Creates a new observable that invokes the specified observer for all emissions by the current
    observable.

    @param observer
        The observer to be invoked for all emissions. It is invoked before the emission is sent to
        downstream observers. If any handler of the observer throws an exception, the new 
        observable will terminate with the thrown exception as an error.

    @return
        An observable that invokes the specified observer for all emissions.

    @throw std::bad_alloc
    */
    OBSERVABLE<T> Do(const OBSERVER<T>& observer) {
        auto observer_core = internal::InsiderT<OBSERVER<T>>::GetCore(observer);
        return OBSERVABLE<T>{ core_->Do(std::move(observer_core)) };
    }

    /**
    Creates a new observable that invokes the specified handler when the current observable emits
    an error.

    @param handler
        The handler to be invoked when the current observable emits an error. It is invoked before
        the error emission is sent to downstream observers. If the handler throws an exception, the
        new observable will terminate with the thrown exception as an error.

    @pre
        The handler is not null.

    @return
        An observable that invokes the specified handler when the current observable emits an 
        error.

    @throw zaf::PreconditionError
    @throw std::bad_alloc
    */
    OBSERVABLE<T> DoOnError(rx::OnError handler) {
        ZAF_EXPECT(handler);
        return static_cast<OBSERVABLE<T>*>(this)->Do(OBSERVER<T>::Create(std::move(handler)));
    }

    /**
    Creates a new observable that invokes the specified handler when the current observable emits 
    an error of the specified type.

    @tparam E
        The type of error to handle.

    @param handler
        The handler to be invoked when the current observable emits an error of type `E`. It is
        invoked before the error emission is sent to downstream observers. If the handler throws
        an exception, the new observable will terminate with the thrown exception as an error.

    @pre
        The handler is not null.

    @return
        An observable that invokes the specified handler when the current observable emits an error
        of type `E`.

    @throw zaf::PreconditionError
    @throw std::bad_alloc

    @details
        The new observable will re-throw the error to check its type. If the error is not of the 
        specified type, it will be ignored and the handler won't be called.

        Note that if there are multiple error type need to be handled, it is better to use the 
        `DoOnError(rx::OnError)` overload rather than chaining multiple `DoOnError<E>` calls, 
        because re-throwing the error multiple times introduces extra overhead.
    */
    template<typename E>
    OBSERVABLE<T> DoOnError(std::function<void(const E&)> handler) {

        ZAF_EXPECT(handler);

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

    /**
    Creates a new observable that switches to another observable provided by the specified handler
    when the current observable emits an error.

    @param handler
        The handler that provides another observable when the current observable emits an error. If
        the handler throws an exception, the returned observable will terminate with the thrown 
        exception as an error.

    @pre 
        The handler is not null.

    @return
        An observable that switches to another observable provided by the specified handler when 
        the current observable emits an error.

    @throw zaf::PreconditionError
    @throw std::bad_alloc
    */
    OBSERVABLE<T> Catch(std::function<OBSERVABLE<T>(const std::exception_ptr&)> handler) {

        ZAF_EXPECT(handler);

        const auto& core = static_cast<OBSERVABLE<T>*>(this)->Core();
        auto new_core = core->Catch(
            [handler = std::move(handler)](const std::exception_ptr& error) {
            return handler(error).Core();
        });
        return OBSERVABLE<T>{ std::move(new_core) };
    }

    /**
    Creates a new observable that invokes the specified handler when the current observable is 
    terminated, either by completion or error.

    @param handler
        The handler to be invoked upon termination of the current observable. It is invoked before
        the termination emission is sent to downstream observers. If the handler throws an 
        exception, the new observable will terminate with the thrown exception as an error.

    @pre
        The handler is not null.

    @return
        An observable that invokes the specified handler upon termination of the current 
        observable.

    @throw zaf::PreconditionError
    @throw std::bad_alloc
    */
    OBSERVABLE<T> DoOnTerminate(Closure handler) {
        const auto& core = static_cast<OBSERVABLE<T>*>(this)->Core();
        return OBSERVABLE<T>{ core->DoOnTerminate(std::move(handler)) };
    }

    /**
    Creates a new observable that invokes the specified handler after the current observable is
    terminated, either by completion or error.

    @param handler
        The handler to be invoked after the current observable is terminated. It is invoked after
        the termination emission is sent to downstream observers. The handler must not throw, 
        otherwise the behavior is undefined.

    @pre
        The handler is not null.

    @return
        An observable that invokes the specified handler after the current observable is 
        terminated.

    @throw zaf::PreconditionError
    @throw std::bad_alloc
    */
    OBSERVABLE<T> DoAfterTerminate(Closure handler) {
        const auto& core = static_cast<OBSERVABLE<T>*>(this)->Core();
        return OBSERVABLE<T>{ core->DoAfterTerminate(std::move(handler)) };
    }

    /**
    Creates a new observable that invokes the specified handler after the current observable is
    terminated or the subscription is disposed.

    @param handler
        The handler to be invoked after the current observable is terminated or the subscription is
        disposed. It is invoked after the termination emission is sent to downstream observers, or
        after the subscription is disposed. The handler must not throw, otherwise the behavior is 
        undefined.

    @pre
        The handler is not null.

    @return
        An observable that invokes the specified handler after the current observable is terminated
        or the subscription is disposed.

    @throw zaf::PreconditionError
    @throw std::bad_alloc
    */
    OBSERVABLE<T> Finally(Closure handler) {
        return OBSERVABLE<T>{ core_->Finally(std::move(handler)) };
    }

    /**
    Creates a new observable that applies a mapping function to each item emitted by the current
    observable, transforming the items into items of another type.

    @tparam K
        The type of items emitted by the new observable.

    @param mapper
        The mapping function to apply to each item emitted by the current observable. It has the
        following signature:
        @code{.cpp}
        K(const T& value);
        @endcode
        If the mapping function throws an exception, the new observable will terminate with the
        thrown exception as an error.

    @pre
        The mapping function is not null.

    @return
        An observable that emits items of type `K`.

    @throw zaf::PreconditionError
    @throw std::bad_alloc
    */
    template<typename K>
    OBSERVABLE<K> Map(std::function<K(const T&)> mapper) {
        ZAF_EXPECT(mapper);
        auto new_core = core_->Map([mapper = std::move(mapper)](const std::any& value) {
            return mapper(std::any_cast<T>(value));
        });
        return OBSERVABLE<K>{ std::move(new_core) };
    }

    /**
    Creates a new observable that applies a mapping function to each item emitted by the current
    observable, transforming the items into observables, and then flattens these observables into
    one observable.

    @tparam K
        The type of items emitted by the new observable.

    @param mapper
        The mapping function to apply to each item emitted by the current observable. It has the
        following signature:
        @code{.cpp}
        OBSERVABLE<K>(const T& value);
        @endcode
        If the mapping function throws an exception, the new observable will terminate with the
        thrown exception as an error.

    @pre
        The mapping function is not null.

    @return
        An observable that emits items of type `K`. It terminates when one of the following
        conditions is met:
        - The current observable and all mapped observables have completed.
        - The current observable or any mapped observable emits an error.

    @throw zaf::PreconditionError
    @throw std::bad_alloc
    */
    template<typename K>
    OBSERVABLE<K> FlatMap(std::function<OBSERVABLE<K>(const T&)> mapper) {
        ZAF_EXPECT(mapper);
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
