#pragma once

#include <zaf/rx/base_observable.h>
#include <zaf/rx/continuous_observer.h>
#include <zaf/rx/internal/observable_factory.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/subscription.h>

namespace zaf::rx::internal {
template<typename T>
class ContinuousFactory;
}

namespace zaf::rx {

template<typename T>
class Continuous : public BaseObservable<Continuous, ContinuousObserver, T> {

    using Base = BaseObservable<Continuous, ContinuousObserver, T>;

public:
    using Base::Do;
    using Base::Subscribe;

    [[nodiscard]] 
    Subscription Subscribe(OnNext<T> on_next) {
        auto observer = ContinuousObserver<T>::Create(std::move(on_next));
        return this->Subscribe(observer);
    }

    Continuous Do(OnNext<T> on_next) {
        auto observer = ContinuousObserver<T>::Create(std::move(on_next));
        return this->Do(observer);
    }

    operator Observable<T>() const noexcept {
        return internal::ObservableFactory<T>::CreateObservable(this->Core());
    }

private:
    friend class BaseObservable<Continuous, ContinuousObserver, T>;
    friend class internal::ContinuousFactory<T>;

    template<
        template<typename> typename OBSERVABLE, 
        template<typename> typename OBSERVER,
        typename K
    >
    friend class BaseObservable;

    explicit Continuous(std::shared_ptr<internal::ObservableCore> core) noexcept :
        Base(std::move(core)) {

    }
};

}