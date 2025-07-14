#pragma once

#include <zaf/rx/base_observable.h>
#include <zaf/rx/internal/observable_factory.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/single_observer.h>
#include <zaf/rx/single_subscriber.h>
#include <zaf/rx/subscription.h>

namespace zaf::rx::internal {
template<typename T>
class SingleFactory;
}

namespace zaf::rx {

template<typename T>
class Single : public BaseObservable<Single, SingleObserver, SingleSubscriber, T> {

    using Base = BaseObservable<Single, SingleObserver, SingleSubscriber, T>;

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
        return internal::ObservableFactory<T>::CreateObservable(this->Core());
    }

private:
    friend Base;
    friend class internal::SingleFactory<T>;

    template<
        template<typename> typename OBSERVABLE, 
        template<typename> typename OBSERVER,
        template<typename> typename SUBSCRIBER,
        typename K
    >
    friend class BaseObservable;

    explicit Single(std::shared_ptr<internal::ObservableCore> core) noexcept : 
        Base(std::move(core)) {

    }
};

}