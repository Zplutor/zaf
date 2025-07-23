#pragma once

#include <memory>
#include <ranges>
#include <zaf/rx/base_observable.h>
#include <zaf/rx/internal/observable/concat_observable.h>
#include <zaf/rx/internal/observable/connectable_observable_core.h>
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

namespace zaf::rx {

class Scheduler;

template<typename T>
class ConnectableObservable;

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

    template<std::ranges::range RANGE>
    static Observable<T> Concat(RANGE&& range) {
        rx::internal::ObservableCoreList observable_cores;
        for (const auto& each_observable : range) {
            observable_cores.push_back(each_observable.Core());
        }
        auto core = std::make_shared<rx::internal::ConcatObservable>(std::move(observable_cores));
        return rx::internal::ObservableFactory<T>::CreateObservable(std::move(core));
    }

    template<typename T>
    static Observable<T> Concat(std::initializer_list<Observable<T>> observables) {
        return Concat<std::initializer_list<Observable<T>>>(std::move(observables));
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

    ConnectableObservable<T> Publish();
    ConnectableObservable<T> Replay();
    ConnectableObservable<T> Replay(std::size_t replay_size);

protected:
    explicit Observable(std::shared_ptr<rx::internal::ObservableCore> core) noexcept :
        Base(std::move(core)) {
    }

private:
    friend Base;
    friend class ConnectableObservable<T>;
    friend class rx::internal::ObservableFactory<T>;

    template<
        template<typename> typename OBSERVABLE,
        template<typename> typename OBSERVER,
        template<typename> typename SUBSCRIBER,
        typename K
    >
    friend class rx::BaseObservable;
};


template<typename T>
class ConnectableObservable : public Observable<T> {
public:
    [[nodiscard]]
    Subscription Connect() {
        auto core = static_cast<internal::ConnectableObservableCore*>(this->Core().get());
        auto sub_core = core->Connect();
        return Subscription{ std::move(sub_core) };
    }

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