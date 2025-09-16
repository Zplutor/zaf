#include <zaf/rx/internal/observable/observable_core.h>
#include <zaf/rx/internal/observable/connectable_observable_core.h>
#include <zaf/rx/internal/operator/catch_operator.h>
#include <zaf/rx/internal/operator/debounce_operator.h>
#include <zaf/rx/internal/operator/do_after_terminate_operator.h>
#include <zaf/rx/internal/operator/do_on_terminate_operator.h>
#include <zaf/rx/internal/operator/do_operator.h>
#include <zaf/rx/internal/operator/finally_operator.h>
#include <zaf/rx/internal/operator/flat_map_operator.h>
#include <zaf/rx/internal/operator/map_operator.h>
#include <zaf/rx/internal/operator/observe_on_operator.h>
#include <zaf/rx/internal/operator/sample_operator.h>
#include <zaf/rx/internal/operator/subscribe_on_operator.h>
#include <zaf/rx/internal/operator/throttle_first_operator.h>
#include <zaf/rx/internal/subject/subject_core_indirect.h>

namespace zaf::rx::internal {

std::shared_ptr<ObservableCore> ObservableCore::SubscribeOn(
    std::shared_ptr<Scheduler> scheduler) {

    return std::make_shared<SubscribeOnOperator>(shared_from_this(), std::move(scheduler));
}


std::shared_ptr<ObservableCore> ObservableCore::ObserveOn(std::shared_ptr<Scheduler> scheduler) {
    return std::make_shared<ObserveOnOperator>(shared_from_this(), std::move(scheduler));
}


std::shared_ptr<ObservableCore> ObservableCore::Do(
    std::shared_ptr<ObserverCore> do_observer) {

    return std::make_shared<DoOperator>(shared_from_this(), std::move(do_observer));
}


std::shared_ptr<ObservableCore> ObservableCore::DoOnTerminate(Closure work) {
    return std::make_shared<DoOnTerminateOperator>(shared_from_this(), std::move(work));
}

std::shared_ptr<ObservableCore> ObservableCore::DoAfterTerminate(Closure work) {
    return std::make_shared<DoAfterTerminateOperator>(shared_from_this(), std::move(work));
}

std::shared_ptr<ObservableCore> ObservableCore::Catch(CatchHandler handler) {
    return std::make_shared<CatchOperator>(shared_from_this(), std::move(handler));
}


std::shared_ptr<ObservableCore> ObservableCore::Finally(Closure work) {
    return std::make_shared<FinallyOperator>(shared_from_this(), std::move(work));
}


std::shared_ptr<ObservableCore> ObservableCore::Map(Mapper mapper) {
    return std::make_shared<MapOperator>(shared_from_this(), std::move(mapper));
}


std::shared_ptr<ObservableCore> ObservableCore::FlatMap(FlatMapper mapper) {
    return std::make_shared<FlatMapOperator>(shared_from_this(), std::move(mapper));
}


std::shared_ptr<ObservableCore> ObservableCore::Debounce(
    std::chrono::steady_clock::duration duration,
    std::shared_ptr<Scheduler> scheduler) {

    return std::make_shared<DebounceOperator>(shared_from_this(), duration, std::move(scheduler));
}


std::shared_ptr<ObservableCore> ObservableCore::Sample(
    std::chrono::steady_clock::duration interval,
    std::shared_ptr<Scheduler> scheduler) {

    return std::make_shared<SampleOperator>(shared_from_this(), interval, std::move(scheduler));
}


std::shared_ptr<ObservableCore> ObservableCore::ThrottleFirst(
    std::chrono::steady_clock::duration duration) {

    return std::make_shared<ThrottleFirstOperator>(shared_from_this(), duration);
}


std::shared_ptr<ConnectableObservableCore> ObservableCore::Publish() {
    return std::make_shared<ConnectableObservableCore>(shared_from_this(), CreateSubjectCore());
}


std::shared_ptr<ConnectableObservableCore> ObservableCore::Replay(
    std::optional<std::size_t> replay_size) {

    return std::make_shared<ConnectableObservableCore>(
        shared_from_this(), 
        CreateReplaySubjectCore(replay_size));
}

}