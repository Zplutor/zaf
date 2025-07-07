#pragma once

#include <zaf/rx/base_observable.h>
#include <zaf/rx/flat_map_capability.h>
#include <zaf/rx/internal/observable_factory.h>
#include <zaf/rx/internal/observable/just_observable.h>
#include <zaf/rx/internal/observable/never_observable.h>
#include <zaf/rx/internal/single_factory.h>
#include <zaf/rx/once_observer.h>
#include <zaf/rx/single.h>
#include <zaf/rx/termination_capability.h>

namespace zaf::rx::internal {
template<typename T>
class OnceFactory;
}

namespace zaf::rx {

template<typename T>
class Once : 
    public BaseObservable<Once, OnceObserver, T>,
    public TerminationCapability<Once<T>>,
    public FlatMapCapability<Once, T> {

    using Base = BaseObservable<Once, OnceObserver, T>;

public:
    static Once<T> Just(T value) {
        return Once<T>{ std::make_shared<internal::JustObservable>(std::any{ std::move(value) }) };
    }

    static Once<T> Never() {
        return Once<T>{ internal::NeverObservable::Instance() };
    }

public:
    using Base::Do;
    using Base::Subscribe;

    [[nodiscard]]
    Subscription Subscribe(OnDone<T> on_done) {
        auto observer = OnceObserver<T>::Create(std::move(on_done));
        return this->Subscribe(std::move(observer));
    }

    Once Do(OnDone<T> on_done) {
        auto observer = OnceObserver<T>::Create(std::move(on_done));
        return this->Do(std::move(observer));
    }

    operator Single<T>() const noexcept {
        return internal::SingleFactory<T>::Create(this->Core());
    }

    operator Observable<T>() const noexcept {
        return internal::ObservableFactory<T>::CreateObservable(this->Core());
    }

private:
    friend Base;
    friend class TerminationCapability<Once<T>>;
    friend class internal::OnceFactory<T>;

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

    using Base::Base;
};

}