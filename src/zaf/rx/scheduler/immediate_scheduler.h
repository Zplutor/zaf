#pragma once

/**
@file
    Defines the `zaf::rx::ImmediateScheduler` class.
*/

#include <zaf/rx/scheduler/scheduler.h>

namespace zaf::rx {

/**
Represents a scheduler that executes works immediately in the current thread.
*/
class ImmediateScheduler : public Scheduler {
public:
    /**
    Gets the singleton instance of the `zaf::rx::ImmediateScheduler`.

    @return
        The singleton instance.

    @post
        The returned object is not null.

    @throw std::bad_alloc
        Thrown if fails to create the instance.
    */
    static const std::shared_ptr<ImmediateScheduler>& Instance();

public:
    /**
    @copydoc zaf::rx::Scheduler::ScheduleWork

    ---
    @details
        This method executes the work immediately in the current thread before returning.
    */
    std::shared_ptr<Disposable> ScheduleWork(Closure work) override;

    /**
    @copydoc zaf::rx::Scheduler::ScheduleDelayedWork

    ---
    @details
        This method sleeps for the specified delay duration first, then executes the work in the 
        current thread before returning.
    */
    std::shared_ptr<Disposable> ScheduleDelayedWork(
        std::chrono::steady_clock::duration delay,
        Closure work) override;

private:
    ImmediateScheduler() noexcept = default;
};

}
