#pragma once

#include <zaf/rx/internal/observable/observable_core.h>
#include <zaf/rx/subscription.h>

namespace zaf::rx {

template<
    template<typename> typename OBSERVABLE, 
    template<typename> typename OBSERVER,
    typename T
>
class BaseObservable {
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
