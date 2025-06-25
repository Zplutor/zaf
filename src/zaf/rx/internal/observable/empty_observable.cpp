#include <zaf/rx/internal/observable/empty_observable.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/subscription/inner_subscription.h>

namespace zaf::internal {

const std::shared_ptr<EmptyObservable>& EmptyObservable::Instance() {
    static std::shared_ptr<EmptyObservable> instance{ new EmptyObservable{} };
    return instance;
}


std::shared_ptr<InnerSubscription> EmptyObservable::Subscribe(
    const std::shared_ptr<ObserverCore>& observer) {

    observer->OnCompleted();
    return InnerSubscription::Empty();
}

}