#include <zaf/rx/timer.h>
#include <zaf/rx/internal/observable_factory.h>
#include <zaf/rx/internal/observable/timer_observable.h>

namespace zaf::rx {
namespace {

Observable<std::size_t> CreateTimerObservable(
    std::chrono::steady_clock::duration delay,
    std::optional<std::chrono::steady_clock::duration> interval,
    std::shared_ptr<Scheduler> scheduler) {

    auto core = std::make_shared<internal::TimerObservable>(
        std::move(delay),
        std::move(interval),
        std::move(scheduler));

    return internal::ObservableFactory<std::size_t>::CreateObservable(std::move(core));
}

} // namespace


Observable<std::size_t> Timer(
    std::chrono::steady_clock::duration delay,
    std::shared_ptr<Scheduler> scheduler) {

    return CreateTimerObservable(std::move(delay), std::nullopt, std::move(scheduler));
}


Observable<std::size_t> Timer(
    std::chrono::steady_clock::duration delay,
    std::chrono::steady_clock::duration interval,
    std::shared_ptr<Scheduler> scheduler) {

    return CreateTimerObservable(std::move(delay), std::move(interval), std::move(scheduler));
}


Observable<std::size_t> Interval(
    std::chrono::steady_clock::duration interval,
    std::shared_ptr<Scheduler> scheduler) {

    return CreateTimerObservable(interval, interval, std::move(scheduler));
}

}