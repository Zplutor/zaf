#include <zaf/rx/internal/observable/empty_observable.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/subscription/subscription_core.h>

namespace zaf::rx::internal {

const std::shared_ptr<EmptyObservable>& EmptyObservable::Instance() {
    static std::shared_ptr<EmptyObservable> instance{ new EmptyObservable{} };
    return instance;
}


std::shared_ptr<SubscriptionCore> EmptyObservable::Subscribe(ObserverShim&& observer) {

    observer.OnCompleted();
    return nullptr;
}

}