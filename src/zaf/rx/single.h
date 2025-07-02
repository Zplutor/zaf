#pragma once

#include <zaf/rx/base_observable.h>
#include <zaf/rx/error_capability.h>
#include <zaf/rx/flat_map_capability.h>
#include <zaf/rx/internal/observable/just_observable.h>
#include <zaf/rx/internal/observable/never_observable.h>
#include <zaf/rx/internal/observable/throw_observable.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/single_observer.h>
#include <zaf/rx/subscription.h>
#include <zaf/rx/termination_capability.h>

namespace zaf::rx::internal {
template<typename T>
class SingleFactory;
}

namespace zaf::rx {

template<typename T>
class Single : 
    public BaseObservable<Single, SingleObserver, T>,
    public ErrorCapability<Single<T>, SingleObserver<T>>,
    public TerminationCapability<Single<T>>,
    public FlatMapCapability<Single, T> {

    using Base = BaseObservable<Single, SingleObserver, T>;

public:
    static Single Just(T value) {
        return Single{ std::make_shared<internal::JustObservable>(std::any{ std::move(value) }) };
    }

    static Single Throw(std::exception_ptr error) {
        return Single{ std::make_shared<internal::ThrowObservable>(std::move(error)) };
    }

    template<typename E>
    static Single Throw(E&& error) {
        return Throw(std::make_exception_ptr(std::forward<E>(error)));
    }

    static Single Never() {
        return Single{ internal::NeverObservable::Instance() };
    }

public:
    using Base::Do;
    using Base::Subscribe;

    [[nodiscard]]
    Subscription Subscribe(OnSuccess<T> on_success) {
        return Subscribe(std::move(on_success), nullptr);
    }

    [[nodiscard]]
    Subscription Subscribe(OnSuccess<T> on_success, OnError on_error) {
        auto observer = SingleObserver<T>::Create(std::move(on_success), std::move(on_error));
        return Subscription{ this->Core()->Subscribe(observer.Core())};
    }

    Single Do(OnSuccess<T> on_success) {
        return Do(SingleObserver<T>::Create(std::move(on_success), nullptr));
    }

    Single Do(OnSuccess<T> on_success, OnError on_error) {
        return Do(SingleObserver<T>::Create(std::move(on_success), std::move(on_error)));
    }

    /**
    Converts a single to an observable implicitly.

    @details
        We define a conversion operator in `Single<>` rather than defining a constructor in
        `Observable<>` to avoid `Observable<>` depending on `Single<>`, as `Single<>` is a
        specialized type of `Observable<>`.
    */
    operator Observable<T>() const noexcept {
        return Observable<T>{ this->Core() };
    }

private:
    friend class BaseObservable<Single, SingleObserver, T>;
    friend class TerminationCapability<Single<T>>;
    friend class internal::SingleFactory<T>;

    template<
        template<typename> typename OBSERVABLE, 
        template<typename> typename OBSERVER,
        typename K
    >
    friend class BaseObservable;

    template<
        template<typename> typename OBSERVABLE,
        typename K
    >
    friend class FlatMapCapability;

    explicit Single(std::shared_ptr<internal::ObservableCore> core) noexcept : 
        Base(std::move(core)) {

    }
};

}