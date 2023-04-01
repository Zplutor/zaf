#include <zaf/rx/internal/observable/customized_observable.h>
#include <zaf/base/error/check.h>
#include <zaf/base/error/error.h>
#include <zaf/rx/internal/inner_observer.h>
#include <zaf/rx/internal/subscription/empty_subscription_core.h>

namespace zaf::internal {

CustomizedObservable::CustomizedObservable(Procedure procedure) : procedure_(std::move(procedure)) {

    ZAF_EXPECT(procedure_);
}


std::shared_ptr<SubscriptionCore> CustomizedObservable::Subscribe(
    const std::shared_ptr<InnerObserver>& observer) {

    try {

        return procedure_(observer);
    }
    catch (const Error& error) {

        observer->OnError(error);
        return EmptySubscriptionCore::Instance();
    }
}

}