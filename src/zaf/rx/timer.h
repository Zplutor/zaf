#pragma once

#include <chrono>
#include <zaf/rx/observable.h>
#include <zaf/rx/schedulers/scheduler.h>
#include <zaf/rx/single.h>

namespace zaf::rx {

/**
Creates an one-shot timer that emits after a delay.
*/
Single<std::size_t> Timer(
    std::chrono::steady_clock::duration delay,
    std::shared_ptr<Scheduler> scheduler);

/**
Creates a timer that emits after a delay and then repeatedly emits at an interval.
*/
Observable<std::size_t> Timer(
    std::chrono::steady_clock::duration delay,
    std::chrono::steady_clock::duration interval,
    std::shared_ptr<Scheduler> scheduler);

/**
Creates a repeating timer that emits at an interval.
*/
Observable<std::size_t> Interval(
    std::chrono::steady_clock::duration interval, 
    std::shared_ptr<Scheduler> scheduler);

}