#include <zaf/rx/internal/observable/never_observable.h>
#include <zaf/rx/internal/subscription/inner_subscription.h>

namespace zaf::internal {

const std::shared_ptr<NeverObservable>& NeverObservable::Instance() {

    static std::shared_ptr<NeverObservable> instance{ new NeverObservable{} };
    return instance;
}


std::shared_ptr<InnerSubscription> NeverObservable::Subscribe(
    const std::shared_ptr<InnerObserver>& observer) {

    return InnerSubscription::Empty();
}

}