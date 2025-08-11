#include <zaf/rx/timer.h>
#include <zaf/rx/internal/observable_factory.h>
#include <zaf/rx/internal/observable/timer_observable.h>
#include <zaf/rx/internal/single_factory.h>

namespace zaf::rx {

Single<std::size_t> Timer(
    std::chrono::steady_clock::duration delay,
    std::shared_ptr<Scheduler> scheduler) {

    auto core = std::make_shared<internal::TimerObservable>(
        delay, 
        std::nullopt,
        std::move(scheduler));

    return internal::SingleFactory<std::size_t>::CreateSingle(std::move(core));
}


Observable<std::size_t> Timer(
    std::chrono::steady_clock::duration delay,
    std::chrono::steady_clock::duration interval,
    std::shared_ptr<Scheduler> scheduler) {

    auto core = std::make_shared<internal::TimerObservable>(
        delay,
        interval,
        std::move(scheduler));

    return internal::ObservableFactory<std::size_t>::CreateObservable(std::move(core));
}


Observable<std::size_t> Interval(
    std::chrono::steady_clock::duration interval,
    std::shared_ptr<Scheduler> scheduler) {

    auto core = std::make_shared<internal::TimerObservable>(
        interval,
        interval,
        std::move(scheduler));

    return internal::ObservableFactory<std::size_t>::CreateObservable(std::move(core));
}

}