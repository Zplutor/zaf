#include <zaf/rx/internal/observable/never_observable.h>
#include <zaf/rx/internal/subscription/subscription_core.h>

namespace zaf::rx::internal {

const std::shared_ptr<NeverObservable>& NeverObservable::Instance() {

    static std::shared_ptr<NeverObservable> instance{ new NeverObservable{} };
    return instance;
}


std::shared_ptr<SubscriptionCore> NeverObservable::Subscribe(
    const std::shared_ptr<ObserverCore>& observer) {

    return SubscriptionCore::Empty();
}

}