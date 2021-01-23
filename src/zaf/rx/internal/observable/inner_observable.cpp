#include <zaf/rx/internal/observable/inner_observable.h>
#include <zaf/rx/internal/operator/observe_on_operator.h>
#include <zaf/rx/internal/operator/subscribe_on_operator.h>

namespace zaf::internal {

std::shared_ptr<InnerObservable> InnerObservable::SubscribeOn(
    std::shared_ptr<Scheduler> scheduler) {

    return std::make_shared<SubscribeOnOperator>(shared_from_this(), std::move(scheduler));
}


std::shared_ptr<InnerObservable> InnerObservable::ObserveOn(std::shared_ptr<Scheduler> scheculer) {

    return std::make_shared<ObserveOnOperator>(shared_from_this(), std::move(scheculer));
}

}