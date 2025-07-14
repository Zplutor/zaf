#pragma once

#include <memory>
#include <zaf/rx/base_observable.h>
#include <zaf/rx/internal/observable/empty_observable.h>
#include <zaf/rx/internal/observable/observable_core.h>
#include <zaf/rx/observer.h>
#include <zaf/rx/observer_functions.h>
#include <zaf/rx/subscriber.h>
#include <zaf/rx/subscription.h>

namespace zaf::rx::internal {
template<typename T>
class ObservableFactory;
}

namespace zaf {

class Scheduler;

template<typename T>
class Observable : public rx::BaseObservable<Observable, Observer, rx::Subscriber, T> {

    using Base = rx::BaseObservable<Observable, Observer, rx::Subscriber, T>;

public:
    /**
    Creates an observable that emits no items but terminates normally.
    */
    static Observable<T> Empty() {
        return Observable<T>{ rx::internal::EmptyObservable::Instance() };
    }

public:
    using Base::Do;
    using Base::Subscribe;

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
        auto observer = Observer<T>::Create(
            std::move(on_next), 
            std::move(on_error), 
            std::move(on_completed));
        return Base::Subscribe(std::move(observer));
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

    Observable DoOnCompleted(OnCompleted on_completed) {
        return Do(nullptr, nullptr, std::move(on_completed));
    }

private:
    friend Base;
    friend class rx::internal::ObservableFactory<T>;

    template<
        template<typename> typename OBSERVABLE,
        template<typename> typename OBSERVER,
        template<typename> typename SUBSCRIBER,
        typename K
    >
    friend class rx::BaseObservable;

    explicit Observable(std::shared_ptr<rx::internal::ObservableCore> core) noexcept :
        Base(std::move(core)) {
    }
};

}