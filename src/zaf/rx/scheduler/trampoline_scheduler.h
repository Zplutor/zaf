#pragma once

/**
@file
    Defines the `zaf::rx::TrampolineScheduler` class.
*/

#include <zaf/rx/scheduler/scheduler.h>

namespace zaf::rx {

/**
Represents a scheduler that executes works in the current thread after the current work is done.

@details
    This scheduler uses thread storage to maintain a work queue for each thread. When a work is 
    scheduled while there is already a work being executed in the current thread, the new work is
    queued and will be executed after the current work and all queued works before it are done. 
    This behavior is different from `zaf::rx::ImmediateScheduler` which always executes works
    immediately and recursively, and it is helpful to avoid stack overflow in some cases.
*/
class TrampolineScheduler : public Scheduler {
public:
    /**
    Gets the singleton instance of the `zaf::rx::TrampolineScheduler`.

    @return 
        The singleton instance.

    @post 
        The returned object is not null.

    @throw std::bad_alloc 
        Thrown if fails to create the instance.
    */
    static const std::shared_ptr<TrampolineScheduler>& Instance();

public:
    /**
    @copydoc zaf::rx::Scheduler::ScheduleWork

    ---
    @throw std::bad_alloc

    @details
        If there is no work is being executed in the current thread, this method executes the work
        immediately before returning. Otherwise, the work is queued and will be executed after the
        current work and all queued works before it are done. When this method returns, all queued
        works are guaranteed to be executed.
    */
    std::shared_ptr<Disposable> ScheduleWork(Closure work) override;

    /**
    @copydoc zaf::rx::Scheduler::ScheduleDelayedWork

    ---
    @throw std::bad_alloc

    @details
        This method is similar to `ScheduleWork()` except that it sleeps for the specified delay
        duration before executing the work.
    */
    std::shared_ptr<Disposable> ScheduleDelayedWork(
        std::chrono::steady_clock::duration delay,
        Closure work) override;

private:
    TrampolineScheduler() noexcept = default;
};

}