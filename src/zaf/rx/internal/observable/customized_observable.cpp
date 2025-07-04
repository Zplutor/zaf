#include <zaf/rx/internal/observable/customized_observable.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/base/error/error.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/subscription/subscription_core.h>

namespace zaf::rx::internal {

CustomizedObservable::CustomizedObservable(Procedure procedure) : procedure_(std::move(procedure)) {

    ZAF_EXPECT(procedure_);
}


std::shared_ptr<SubscriptionCore> CustomizedObservable::Subscribe(
    const std::shared_ptr<ObserverCore>& observer) {

    try {
        return procedure_(observer);
    }
    catch (...) {
        observer->OnError(std::current_exception());
        return nullptr;
    }
}

}