#pragma once

/**
@file
    Defines the `zaf::rx::Observable<>` and `zaf::rx::ConnectableObservable<>` class templates.
*/

#include <memory>
#include <ranges>
#include <zaf/rx/internal/insider/insider.h>
#include <zaf/rx/internal/insider/observer_insider.h>
#include <zaf/rx/internal/observable/concat_observable.h>
#include <zaf/rx/internal/observable/connectable_observable_core.h>
#include <zaf/rx/internal/observable/empty_observable.h>
#include <zaf/rx/internal/observable/observable_core.h>
#include <zaf/rx/observable_base.h>
#include <zaf/rx/observer.h>
#include <zaf/rx/observer_functions.h>
#include <zaf/rx/subscriber.h>

namespace zaf::rx::internal {
class ObservableInsider;
}

namespace zaf::rx {

class Scheduler;

template<typename T>
class ConnectableObservable;

/**
Represents a stream of data or events that can be observed.
*/
template<typename T>
class Observable : public ObservableBase<Observable, Observer, Subscriber, T> {

    using Base = ObservableBase<Observable, Observer, Subscriber, T>;

public:
    /**
    Creates an observable that emits no items but terminates normally.

    @return 
        An observable that emits no items but terminates normally.

    @throw std::bad_alloc
    */
    static Observable<T> Empty() {
        return Observable<T>{ rx::internal::EmptyObservable::Instance() };
    }

    /**
    Creates an observable that concatenates the observables in the specified range.

    @tparam RANGE
        The type of the range. The value type of the range must be `zaf::rx::Observable<T>` or 
        `zaf::rx::Single<T>`.

    @param range
        The range that contains the observables to be concatenated. The observables in the range
        will be subscribed one by one. The next observable will be subscribed only after the
        previous observable completes.

    @return
        An observable that concatenates the observables in the specified range. It terminates when 
        one of the following conditions is met:
        - The last observable in the range completes.
        - Any observable in the range emits an error.

    @throw std::bad_alloc
    */
    template<std::ranges::range RANGE>
    static Observable<T> Concat(RANGE&& range) {
        using RangeValueType = std::ranges::range_value_t<RANGE>;
        rx::internal::ObservableCoreList observable_cores;
        for (const auto& each_observable : range) {
            auto core = internal::InsiderT<RangeValueType>::GetCore(each_observable);
            observable_cores.push_back(std::move(core));
        }
        auto core = std::make_shared<rx::internal::ConcatObservable>(std::move(observable_cores));
        return Observable<T>{ std::move(core) };
    }

    /**
    Creates an observable that concatenates the observables in the specified initializer list.

    @param observables
        The initializer list that contains the observables to be concatenated.

    @return
        An observable that concatenates the observables in the specified initializer list.

    @throw std::bad_alloc

    @details
        This overload method is a convenience for calling `Concat` with an initializer list.

    @see zaf::rx::Observable<>::Concat(RANGE&&)
    */
    static Observable<T> Concat(std::initializer_list<Observable<T>> observables) {
        return Concat<std::initializer_list<Observable<T>>>(std::move(observables));
    }

public:
    using Base::Do;
    using Base::Subscribe;

    /**
    Subscribes to the current observable with the specified item handler, ignoring the error and
    completion emissions.

    @param on_next
        The handler to be invoked when an item is emitted.

    @pre 
        The handler is not null.

    @return
        A disposable object that can be used to unsubscribe from the observable. Callers should
        retain the returned object so that the subscription remains active.

    @post
        The return object is not null.

    @throw zaf::PreconditionError
    @throw std::bad_alloc
    @throw ...
        Any exception thrown by the underlying observable implementation when subscribing.
    */
    [[nodiscard]]
    std::shared_ptr<Disposable> Subscribe(OnNext<T> on_next) {
        ZAF_EXPECT(on_next);
        return Subscribe(std::move(on_next), nullptr, nullptr);
    }

    /**
    Subscribes to the current observable with the specified item handler and error handler, 
    ignoring the completion emission.

    @param on_next
        The handler to be invoked when an item is emitted.

    @param on_error
        The handler to be invoked when an error is emitted.

    @pre 
        - The item handler is not null.
        - The error handler is not null.

    @return
        A disposable object that can be used to unsubscribe from the observable. Callers should
        retain the returned object so that the subscription remains active.

    @post
        The return object is not null.

    @throw zaf::PreconditionError
    @throw std::bad_alloc
    @throw ...
        Any exception thrown by the underlying observable implementation when subscribing.
    */
    [[nodiscard]]
    std::shared_ptr<Disposable> Subscribe(OnNext<T> on_next, OnError on_error) {
        ZAF_EXPECT(on_next);
        ZAF_EXPECT(on_error);
        return Subscribe(std::move(on_next), std::move(on_error), nullptr);
    }

    /**
    Subscribes to the current observable with the specified item handler and completion handler,
    ignoring the error emission.

    @param on_next
        The handler to be invoked when an item is emitted.

    @param on_completed
        The handler to be invoked when the observable completes.

    @pre
        - The item handler is not null.
        - The completion handler is not null.

    @return
        A disposable object that can be used to unsubscribe from the observable. Callers should
        retain the returned object so that the subscription remains active.

    @post
        The return object is not null.

    @throw zaf::PreconditionError
    @throw std::bad_alloc
    @throw ...
        Any exception thrown by the underlying observable implementation when subscribing.
    */
    [[nodiscard]]
    std::shared_ptr<Disposable> Subscribe(OnNext<T> on_next, OnCompleted on_completed) {
        ZAF_EXPECT(on_next);
        ZAF_EXPECT(on_completed);
        return Subscribe(std::move(on_next), nullptr, std::move(on_completed));
    }

    /**
    Subscribes to the current observable with the specified item handler, error handler, and
    completion handler.

    @param on_next
        The handler to be invoked when an item is emitted.

    @param on_error
        The handler to be invoked when an error is emitted.

    @param on_completed
        The handler to be invoked when the observable completes.

    @return
        A disposable object that can be used to unsubscribe from the observable. Callers should
        retain the returned object so that the subscription remains active.

    @post
        The return object is not null.

    @throw std::bad_alloc
    @throw ...
        Any exception thrown by the underlying observable implementation when subscribing.

    @details
        All handlers can be null, in which case the emission will be ignored.
    */
    [[nodiscard]]
    std::shared_ptr<Disposable> Subscribe(
        OnNext<T> on_next, 
        OnError on_error,
        OnCompleted on_completed) {

        auto observer = Observer<T>::Create(
            std::move(on_next), 
            std::move(on_error), 
            std::move(on_completed));

        return Base::Subscribe(std::move(observer));
    }

    /**
    Creates a new observable that invokes the specified handler for each item emission by the 
    current observable.

    @param on_next
        The handler to be invoked when an item is emitted. It is invoked before the item emission
        is sent to downstream observers. If the handler throws an exception, the new observable
        will terminate with the thrown exception as an error.

    @pre 
        The handler is not null.

    @return
        An observable that invokes the specified handler for each item emission by the current
        observable.

    @throw zaf::PreconditionError
    @throw std::bad_alloc
    */
    Observable Do(OnNext<T> on_next) {
        ZAF_EXPECT(on_next);
        return Do(std::move(on_next), nullptr, nullptr);
    }

    /**
    Creates a new observable that invokes the specified handlers for each item emission and the 
    error emission by the current observable.

    @param on_next
        The handler to be invoked when an item is emitted.

    @param on_error
        The handler to be invoked when an error is emitted.

    @pre
        - The item handler is not null.
        - The error handler is not null.

    @return
        An observable that invokes the specified handlers for each item emission and the error
        emission by the current observable.

    @throw zaf::PreconditionError
    @throw std::bad_alloc

    @details
        Handlers are invoked before the emission is sent to downstream observers. If the handler
        throws an exception, the new observable will terminate with the thrown exception as an
        error.
    */
    Observable Do(OnNext<T> on_next, OnError on_error) {
        ZAF_EXPECT(on_next);
        ZAF_EXPECT(on_error);
        return Do(std::move(on_next), std::move(on_error), nullptr);
    }

    /**
    Creates a new observable that invokes the specified handlers for each item emission and the
    completion emission by the current observable.

    @param on_next
        The handler to be invoked when an item is emitted.

    @param on_completed
        The handler to be invoked when the observable completes.

    @pre
        - The item handler is not null.
        - The completion handler is not null.

    @return
        An observable that invokes the specified handlers for each item emission and the completion
        emission by the current observable.

    @throw zaf::PreconditionError
    @throw std::bad_alloc

    @details
        Handlers are invoked before the emission is sent to downstream observers. If the handler
        throws an exception, the new observable will terminate with the thrown exception as an
        error.
    */
    Observable Do(OnNext<T> on_next, OnCompleted on_completed) {
        ZAF_EXPECT(on_next);
        ZAF_EXPECT(on_completed);
        return Do(std::move(on_next), nullptr, std::move(on_completed));
    }

    /**
    Creates a new observable that invokes the specified handlers for all emissions by the current
    observable.

    @param on_next
        The handler to be invoked when an item is emitted.

    @param on_error
        The handler to be invoked when an error is emitted.

    @param on_completed
        The handler to be invoked when the observable completes.

    @return
        An observable that invokes the specified handlers for all emissions by the current
        observable.

    @throw std::bad_alloc

    @details
        All handlers can be null, in which case the handler will be ignored. 

        Handlers are invoked before the emission is sent to downstream observers. If the handler 
        throws an exception, the new observable will terminate with the thrown exception as an 
        error.
    */
    Observable Do(OnNext<T> on_next, OnError on_error, OnCompleted on_completed) {
        return Do(Observer<T>::Create(
            std::move(on_next),
            std::move(on_error),
            std::move(on_completed)));
    }

    /**
    Creates a new observable that invokes the specified handler for the completion emission by the
    current observable.

    @param on_completed
        The handler to be invoked when the observable completes. It is invoked before the 
        completion emission is sent to downstream observers. If the handler throws an exception, 
        the new observable will terminate with the thrown exception as an error.

    @pre
        The handler is not null.

    @return
        An observable that invokes the specified handler for the completion emission by the current
        observable.

    @throw zaf::PreconditionError
    @throw std::bad_alloc
    */
    Observable DoOnCompleted(OnCompleted on_completed) {
        ZAF_EXPECT(on_completed);
        return Do(nullptr, nullptr, std::move(on_completed));
    }

    /**
    Creates a new observable that emits items only after a specified duration has passed without 
    any new items being emitted.

    @param duration
        The duration to wait before emitting the item.

    @param scheduler
        The scheduler to use for the debounce timer. Items will be emitted on this scheduler.

    @pre
        The scheduler is not null.

    @return
        An observable that emits items only after the specified duration has passed without any new
        items being emitted.

    @throw zaf::PreconditionError
    @throw std::bad_alloc

    @details
        Emissions of the returned observable may be sent on different contexts, as described below:
        - Item emissions after debouncing are sent on the specified scheduler.
        - Error and completion emissions are sent immediately.
        - If the observable completes while there is a pending item to be emitted after debouncing,
          the pending item will be emitted immediately before the completion emission.
        
        To ensure that all emissions are sent on the same context, consider using the `ObserveOn`
        operator after this operator.
    */
    Observable<T> Debounce(
        std::chrono::steady_clock::duration duration,
        std::shared_ptr<Scheduler> scheduler) {

        auto new_core = this->Core()->Debounce(duration, std::move(scheduler));
        return Observable<T>{ std::move(new_core) };
    }

    /**
    Creates a connectable observable that shares a single subscription to the current observable.

    @return
        A connectable observable to the current observable.

    @throw std::bad_alloc
    */
    ConnectableObservable<T> Publish();

    /**
    Creates a connectable observable that shares a single subscription to the current observable
    and replays all emitted items to new subscribers.

    @return
        A connectable observable to the current observable, replays all emitted items to new 
        subscribers.

    @throw std::bad_alloc
    */
    ConnectableObservable<T> Replay();

    /**
    Creates a connectable observable that shares a single subscription to the current observable
    and replays a specified number of the most recent emitted items to new subscribers.

    @param replay_size
        The number of the most recent emitted items to replay to new subscribers.

    @return
        A connectable observable to the current observable, replays a specified number of the most
        recent emitted items to new subscribers.

    @throw std::bad_alloc
    */
    ConnectableObservable<T> Replay(std::size_t replay_size);

protected:
    explicit Observable(std::shared_ptr<rx::internal::ObservableCore> core) noexcept :
        Base(std::move(core)) {
    }

private:
    friend Base;
    friend class ConnectableObservable<T>;
    friend class rx::internal::ObservableInsider;

    template<
        template<typename> typename OBSERVABLE,
        template<typename> typename OBSERVER,
        template<typename> typename SUBSCRIBER,
        typename K
    >
    friend class rx::ObservableBase;
};


/**
Represents an observable that can be connected to its source observable, allowing multiple
subscribers to share the same emissions.
*/
template<typename T>
class ConnectableObservable : public Observable<T> {
public:
    /**
    Connects the current connectable observable to its source observable, starting the emission of
    items.

    @return
        A disposable object that can be used to disconnect from the source observable. Callers 
        should retain the returned object so that the connection remains active.

    @post
        The return object is not null.

    @throw ...
        Any exception thrown by the source observable when subscribing to it.
    */
    [[nodiscard]]
    std::shared_ptr<Disposable> Connect() {
        auto core = static_cast<internal::ConnectableObservableCore*>(this->Core().get());
        auto sub_core = core->Connect();
        return std::move(sub_core);
    }

    /**
    Creates an ordinary observable that automatically connects to the source observable when the first
    subscriber subscribes, and disconnects when the last subscriber unsubscribes.

    @return
        An ordinary observable.

    @throw std::bad_alloc
    */
    Observable<T> RefCount() {
        auto core = static_cast<internal::ConnectableObservableCore*>(this->Core().get());
        return Observable<T>{ core->RefCount() };
    }

private:
    friend class Observable<T>;

    explicit ConnectableObservable(
        std::shared_ptr<internal::ConnectableObservableCore> core) noexcept 
        :
        Observable<T>(std::move(core)) {

    }
};


template<typename T>
ConnectableObservable<T> Observable<T>::Publish() {
    auto core = this->Core()->Publish();
    return ConnectableObservable<T>{ std::move(core) };
}

template<typename T>
ConnectableObservable<T> Observable<T>::Replay() {
    auto core = this->Core()->Replay(std::nullopt);
    return ConnectableObservable<T>{ std::move(core) };
}

template<typename T>
ConnectableObservable<T> Observable<T>::Replay(std::size_t replay_size) {
    auto core = this->Core()->Replay(replay_size);
    return ConnectableObservable<T>{ std::move(core) };
}

}
