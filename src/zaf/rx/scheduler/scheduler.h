#pragma once

/**
@file
    Defines the `zaf::rx::Scheduler` class.
*/

#include <chrono>
#include <zaf/base/closure.h>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/disposable.h>

namespace zaf::rx {

/**
An interface for scheduling works to be executed.

@details
    A scheduler is responsible for controlling when and where a piece of work is executed. 
    Different implementations may execute works in different threads and different timings. For 
    example, the `zaf::rx::MainThreadScheduler` posts works to the main thread to execute, while 
    the `zaf::rx::ImmediateScheduler` executes works immediately in the current thread.
*/
class Scheduler : NonCopyableNonMovable {
public:
    Scheduler() = default;
    virtual ~Scheduler() = default;

    /**
    Schedules a work to be executed.

    @param work
        The work to be executed. It must not throw, otherwise the behavior is undefined.

    @pre
        The work is not null.

    @return
        A disposable object that can be used to cancel the scheduled work if it is not being 
        executed.

    @post
        The returned object is not null.

    @throw zaf::PreconditionError
    @throw ...
        Any exception thrown by the scheduler implementations.

    @details
        It is guaranteed that if an exception is thrown, the work is not scheduled and won't be 
        executed.
    */
    virtual std::shared_ptr<Disposable> ScheduleWork(Closure work) = 0;

    /**
    Schedules a work to be executed after a delay.

    @param delay
        The delay before executing the work.

    @param work
        The work to be executed. It must not throw, otherwise the behavior is undefined.

    @pre 
        The work is not null.

    @return
        A disposable object that can be used to cancel the scheduled work if it is not being
        executed.

    @post
        The returned object is not null.

    @throw zaf::PreconditionError
    @throw ...
        Any exception thrown by the scheduler implementations.

    @details
        It is guaranteed that if an exception is thrown, the work is not scheduled and won't be 
        executed.
    */
    virtual std::shared_ptr<Disposable> ScheduleDelayedWork(
        std::chrono::steady_clock::duration delay, 
        Closure work) = 0;
};

}