#include <zaf/rx/internal/observable/throw_observable.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/subscription/inner_subscription.h>

namespace zaf::internal {

ThrowObservable::ThrowObservable(std::exception_ptr error) : error_(std::move(error)) {

}


std::shared_ptr<InnerSubscription> ThrowObservable::Subscribe(
    const std::shared_ptr<ObserverCore>& observer) {

    observer->OnError(error_);
    return InnerSubscription::Empty();
}

}