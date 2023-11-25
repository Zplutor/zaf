#include <zaf/rx/internal/observable/inner_observable.h>
#include <zaf/rx/internal/operator/do_operator.h>
#include <zaf/rx/internal/operator/finally_operator.h>
#include <zaf/rx/internal/operator/flat_map_operator.h>
#include <zaf/rx/internal/operator/map_operator.h>
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


std::shared_ptr<InnerObservable> InnerObservable::Do(
    std::shared_ptr<InnerObserver> do_observer) {

    return std::make_shared<DoOperator>(shared_from_this(), std::move(do_observer));
}


std::shared_ptr<InnerObservable> InnerObservable::Finally(Work work) {

    return std::make_shared<FinallyOperator>(shared_from_this(), std::move(work));
}


std::shared_ptr<InnerObservable> InnerObservable::Map(Mapper mapper) {

    return std::make_shared<MapOperator>(shared_from_this(), std::move(mapper));
}


std::shared_ptr<InnerObservable> InnerObservable::FlatMap(FlatMapper mapper) {

    return std::make_shared<FlatMapOperator>(shared_from_this(), std::move(mapper));
}

}