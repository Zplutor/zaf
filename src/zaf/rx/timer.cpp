#include <zaf/rx/timer.h>
#include <zaf/base/error/precondition_error.h>
#include <zaf/rx/internal/insider/observable_insider.h>
#include <zaf/rx/internal/insider/single_insider.h>
#include <zaf/rx/internal/observable/timer_observable.h>

namespace zaf::rx {

Single<std::size_t> Timer::Once(
    std::chrono::steady_clock::duration delay,
    std::shared_ptr<Scheduler> scheduler) {

    ZAF_EXPECT(scheduler);

    auto core = std::make_shared<internal::TimerObservable>(
        delay, 
        std::nullopt,
        std::move(scheduler));

    return internal::SingleInsider::Create<std::size_t>(std::move(core));
}


Observable<std::size_t> Timer::Interval(
    std::chrono::steady_clock::duration interval,
    std::shared_ptr<Scheduler> scheduler) {

    ZAF_EXPECT(scheduler);

    auto core = std::make_shared<internal::TimerObservable>(
        interval,
        interval,
        std::move(scheduler));

    return internal::ObservableInsider::Create<std::size_t>(std::move(core));
}


Observable<std::size_t> Timer::DelayInterval(
    std::chrono::steady_clock::duration delay,
    std::chrono::steady_clock::duration interval,
    std::shared_ptr<Scheduler> scheduler) {

    ZAF_EXPECT(scheduler);

    auto core = std::make_shared<internal::TimerObservable>(
        delay,
        interval,
        std::move(scheduler));

    return internal::ObservableInsider::Create<std::size_t>(std::move(core));
}

}