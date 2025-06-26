#include <zaf/rx/internal/observable/just_observable.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/subscription/inner_subscription.h>

namespace zaf::rx::internal {

JustObservable::JustObservable(std::any value) : value_(std::move(value)) {

}


std::shared_ptr<InnerSubscription> JustObservable::Subscribe(
    const std::shared_ptr<ObserverCore>& observer) {

    observer->OnNext(value_);
    observer->OnCompleted();
    return InnerSubscription::Empty();
}

}