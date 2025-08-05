#include <zaf/rx/timer.h>
#include <zaf/rx/internal/observable_factory.h>
#include <zaf/rx/internal/observable/timer_observable.h>
#include <zaf/rx/scheduler/timer_scheduler.h>

namespace zaf::rx {
namespace {

Observable<int> CreateTimerObservable(
    std::chrono::steady_clock::duration delay,
    std::optional<std::chrono::steady_clock::duration> interval,
    std::shared_ptr<Scheduler> scheduler) {

    auto core = std::make_shared<internal::TimerObservable>(
        std::move(delay),
        std::move(interval),
        std::move(scheduler));

    return internal::ObservableFactory<int>::CreateObservable(std::move(core));
}

}

Observable<int> Timer(std::chrono::steady_clock::duration delay) {
    return CreateTimerObservable(std::move(delay), std::nullopt, TimerScheduler::Instance());
}


Observable<int> Timer(
    std::chrono::steady_clock::duration delay,
    std::shared_ptr<Scheduler> scheduler) {

    return CreateTimerObservable(std::move(delay), std::nullopt, std::move(scheduler));
}


Observable<int> Timer(
    std::chrono::steady_clock::duration delay,
    std::chrono::steady_clock::duration interval) {

    return CreateTimerObservable(
        std::move(delay), 
        std::move(interval), 
        TimerScheduler::Instance());
}


Observable<int> Timer(
    std::chrono::steady_clock::duration delay,
    std::chrono::steady_clock::duration interval,
    std::shared_ptr<Scheduler> scheduler) {

    return CreateTimerObservable(std::move(delay), std::move(interval), std::move(scheduler));
}


Observable<int> Interval(std::chrono::steady_clock::duration interval) {
    return CreateTimerObservable(interval, interval, TimerScheduler::Instance());
}


Observable<int> Interval(
    std::chrono::steady_clock::duration interval,
    std::shared_ptr<Scheduler> scheduler) {

    return CreateTimerObservable(interval, interval, std::move(scheduler));
}

}