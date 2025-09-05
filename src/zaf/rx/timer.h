#pragma once

/**
@file
    Defines the `zaf::rx::Timer` class.
*/

#include <chrono>
#include <zaf/rx/observable.h>
#include <zaf/rx/scheduler/scheduler.h>
#include <zaf/rx/single.h>

namespace zaf::rx {

/**
Provides factory methods to create timers.
*/
class Timer {
public:
    /**
    Creates an one-shot timer that emits after a delay.

    @param delay
        The delay before the timer emits.

    @param scheduler
        The scheduler on which the timer runs.

    @pre
        The scheduler is not null.

    @return
        A single that emits 0 on the specified scheduler after the specified delay. No error nor 
        completion will be emitted. When subscribing to the single, any exception will be thrown if 
        the timer fails to start on the specified scheduler.

    @throw zaf::PreconditionError
    @throw std::bad_alloc
    */
    static Single<std::size_t> Once(
        std::chrono::steady_clock::duration delay,
        std::shared_ptr<Scheduler> scheduler);

    /**
    Creates a repeating timer that emits at an interval.

    @param interval
        The interval at which the timer emits.

    @param scheduler
        The scheduler on which the timer runs.

    @pre
        The scheduler is not null.

    @return
        An observable that emits increasing numbers starting from 0 on the specified scheduler at
        the specified interval. An error will be emitted if the timer fails to set subsequent 
        intervals after the first emission. No completion will be emitted. When subscribing to the 
        observable, any exception will be thrown if the timer fails to start on the specified 
        scheduler.

    @throw zaf::PreconditionError
    @throw std::bad_alloc
    */
    static Observable<std::size_t> Interval(
        std::chrono::steady_clock::duration interval,
        std::shared_ptr<Scheduler> scheduler);

    /**
    Creates a timer that emits after a delay and then repeatedly emits at an interval.

    @param delay
        The delay before the timer emits the first value.

    @param interval
        The interval at which the timer emits subsequent values.

    @param scheduler
        The scheduler on which the timer runs.

    @pre
        The scheduler is not null.

    @return
        An observable that emits increasing numbers starting from 0 on the specified scheduler 
        after the specified delay and then at the specified interval. An error will be emitted if 
        the timer fails to set subsequent intervals after the first emission. No completion will be
        emitted. When subscribing to the observable, any exception will be thrown if the timer 
        fails to start on the specified scheduler.

    @throw zaf::PreconditionError
    @throw std::bad_alloc
    */
    static Observable<std::size_t> DelayInterval(
        std::chrono::steady_clock::duration delay,
        std::chrono::steady_clock::duration interval,
        std::shared_ptr<Scheduler> scheduler);

private:
    Timer() = delete;
};

}