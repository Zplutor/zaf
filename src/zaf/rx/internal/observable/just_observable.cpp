#include <zaf/rx/internal/observable/just_observable.h>
#include <zaf/rx/internal/inner_observer.h>
#include <zaf/rx/internal/subscription/empty_subscription_core.h>

namespace zaf::internal {

JustObservable::JustObservable(std::any value) : value_(std::move(value)) {

}


std::shared_ptr<SubscriptionCore> JustObservable::Subscribe(
    const std::shared_ptr<InnerObserver>& observer) {

    observer->OnNext(value_);
    observer->OnCompleted();
    return EmptySubscriptionCore::Instance();
}

}