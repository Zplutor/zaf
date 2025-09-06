#pragma once

/**
@file
    Defines the `zaf::rx::Single<>` class template.
*/

#include <zaf/rx/internal/insider/observable_insider.h>
#include <zaf/rx/internal/insider/single_observer_insider.h>
#include <zaf/rx/observable_base.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/single_observer.h>
#include <zaf/rx/single_subscriber.h>

namespace zaf::rx::internal {
class SingleInsider;
}

namespace zaf::rx {

/**
A variant of observable that emits a single item or an error.

@tparam T
    The type of the single item emitted by the single.
*/
template<typename T>
class Single : public ObservableBase<Single, SingleObserver, SingleSubscriber, T> {

    using Base = ObservableBase<Single, SingleObserver, SingleSubscriber, T>;

public:
    using Base::Do;
    using Base::Subscribe;

    /**
    Subscribes to the current single with the specified success handler, ignoring the error 
    emission.

    @param on_success
        The handler to be invoked when the single emits the item.

    @pre 
        The handler is not null.

    @return
        A disposable object that can be used to unsubscribe from the single. Callers should retain
        the returned object so that the subscription remains active.

    @post
        The returned object is not null.

    @throw zaf::PreconditionError
    @throw std::bad_alloc
    @throw ...
        Any exception thrown by the underlying single implementation when subscribing.
    */
    [[nodiscard]]
    std::shared_ptr<Disposable> Subscribe(OnSuccess<T> on_success) {
        ZAF_EXPECT(on_success);
        return Subscribe(std::move(on_success), nullptr);
    }

    /**
    Subscribes to the current single with the specified success handler and error handler.

    @param on_success
        The handler to be invoked when the single emits the item.

    @param on_error
        The handler to be invoked when the single emits an error.

    @return
        A disposable object that can be used to unsubscribe from the single. Callers should retain
        the returned object so that the subscription remains active.

    @throw std::bad_alloc
    @throw ...
        Any exception thrown by the underlying single implementation when subscribing.

    @details
        The success handler and the error handler can be null, in which case the corresponding
        emission will be ignored.
    */
    [[nodiscard]]
    std::shared_ptr<Disposable> Subscribe(OnSuccess<T> on_success, OnError on_error) {
        auto observer = SingleObserver<T>::Create(std::move(on_success), std::move(on_error));
        auto observer_core = internal::SingleObserverInsider::GetCore(observer);
        auto shim = internal::ObserverShim::FromShared(std::move(observer_core));
        return this->Core()->Subscribe(std::move(shim));
    }

    /**
    Creates a new single that invokes the specified success handler for the item emission by the
    current single.

    @param on_success
        The handler to be invoked when the single emits the item.

    @pre
        The handler is not null.

    @return
        A single that invokes the specified success handler for the item emission by the
        current single.

    @throw zaf::PreconditionError
    @throw std::bad_alloc
    */
    Single Do(OnSuccess<T> on_success) {
        ZAF_EXPECT(on_success);
        return Do(SingleObserver<T>::Create(std::move(on_success), nullptr));
    }

    /**
    Creates a new single that invokes the specified handlers for the emissions by the current 
    single.

    @param on_success
        The handler to be invoked when the single emits the item.

    @param on_error
        The handler to be invoked when the single emits an error.

    @return
        A new single that invokes the specified handlers for the emissions by the current single.

    @throw std::bad_alloc

    @details
        The success handler and the error handler can be null, in which case the corresponding
        emission will be ignored.
    */
    Single Do(OnSuccess<T> on_success, OnError on_error) {
        return Do(SingleObserver<T>::Create(std::move(on_success), std::move(on_error)));
    }

    /**
    Converts the current single to an observable implicitly.

    @return
        An observable that emits the item and then completes when the current single emits an item,
        or emits an error when the current single emits an error.

    @details
        We define a conversion operator in `Single<>` rather than defining a constructor in
        `Observable<>` to avoid `Observable<>` depending on `Single<>`, as `Single<>` is a
        specialized type of `Observable<>`.
    */
    operator Observable<T>() const noexcept {
        return internal::ObservableInsider::Create<T>(this->Core());
    }

private:
    friend Base;
    friend class internal::SingleInsider;

    template<
        template<typename> typename OBSERVABLE, 
        template<typename> typename OBSERVER,
        template<typename> typename SUBSCRIBER,
        typename K
    >
    friend class ObservableBase;

    explicit Single(std::shared_ptr<internal::ObservableCore> core) noexcept : 
        Base(std::move(core)) {

    }
};

}
