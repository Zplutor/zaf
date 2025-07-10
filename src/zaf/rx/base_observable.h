#pragma once

#include <zaf/rx/internal/observable/custom_observable.h>
#include <zaf/rx/internal/observable/never_observable.h>
#include <zaf/rx/internal/observable/observable_core.h>
#include <zaf/rx/subscription.h>

namespace zaf::rx {

template<
    template<typename> typename OBSERVABLE,
    template<typename> typename OBSERVER,
    template<typename> typename SUBSCRIBER,
    typename T
>
class BaseObservable {
public:
    /**
    Creates an observable that emits no items and does not terminate.
    */
    static OBSERVABLE<T> Never() {
        return OBSERVABLE<T>{ internal::NeverObservable::Instance() };
    }

    static OBSERVABLE<T> Create(std::function<void(SUBSCRIBER<T> subscriber)> producer) {

        auto bridged_producer = [producer = std::move(producer)](
            std::shared_ptr<internal::ObserverCore> observer_core,
            std::shared_ptr<internal::SubscriptionCore> subscription_core) {

            auto subscriber = SUBSCRIBER<T>{ 
                std::move(observer_core),
                std::move(subscription_core) 
            };
            producer(std::move(subscriber));
        };

        auto core = std::make_shared<internal::CustomObservable>(std::move(bridged_producer));
        return OBSERVABLE<T>{ std::move(core) };
    }

public:
    [[nodiscard]]
    Subscription Subscribe(const OBSERVER<T>& observer) {
        return Subscription{ core_->Subscribe(observer.Core()) };
    }

    [[nodiscard]]
    Subscription Subscribe() {
        return Subscribe(OBSERVER<T>::Create());
    }

    OBSERVABLE<T> SubscribeOn(std::shared_ptr<Scheduler> scheduler) {
        return OBSERVABLE<T>{ core_->SubscribeOn(std::move(scheduler)) };
    }

    OBSERVABLE<T> ObserveOn(std::shared_ptr<Scheduler> scheduler) {
        return OBSERVABLE<T>{ core_->ObserveOn(std::move(scheduler)) };
    }

    OBSERVABLE<T> Do(const OBSERVER<T>& observer) {
        return OBSERVABLE<T>{ core_->Do(observer.Core()) };
    }

    OBSERVABLE<T> Finally(Work work) {
        return OBSERVABLE<T>{ core_->Finally(std::move(work)) };
    }

    template<typename K>
    OBSERVABLE<K> Map(std::function<K(const T&)> mapper) {
        auto new_core = core_->Map([mapper = std::move(mapper)](const std::any& value) {
            return mapper(std::any_cast<T>(value));
        });
        return OBSERVABLE<K>{ std::move(new_core) };
    }

    const std::shared_ptr<internal::ObservableCore>& Core() const noexcept {
        return core_;
    }

protected:
    explicit BaseObservable(std::shared_ptr<internal::ObservableCore> core) noexcept :
        core_(std::move(core)) {

    }

private:
    std::shared_ptr<internal::ObservableCore> core_;
};

}
