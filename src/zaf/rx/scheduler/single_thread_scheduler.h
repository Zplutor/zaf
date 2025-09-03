#pragma once

/**
@file
    Defines the `zaf::rx::SingleThreadScheduler` class.
*/

#include <zaf/rx/scheduler/scheduler.h>
#include <zaf/rx/thread/run_loop_thread.h>

namespace zaf::rx {

/**
Represents a scheduler that executes works on a single thread.
*/
class SingleThreadScheduler : public Scheduler {
public:
    /**
    Constructs a instance that uses a new created thread to execute works.

    @throw std::bad_alloc
    @throw std::system_error
        Thrown if the thread cannot be created.
    */
    SingleThreadScheduler();

    /**
    Constructs an instance that uses the specified thread to execute works.

    @param thread
        The thread to execute works.

    @pre
        The thread is not null.

    @throw zaf::PreconditionError
    */
    explicit SingleThreadScheduler(std::shared_ptr<RunLoopThread> thread);

    /**
    @copydoc zaf::rx::Scheduler::ScheduleWork

    ---
    @throw zaf::rx::ExecutionStoppedError
        The scheduler is stopped. This may occur if the scheduler is being destructed while an 
        executing work tries to schedule a new work.

    @throw ...
        Any exception thrown by the underlying thread implementation.

    @details
        This method delegates to `zaf::rx::RunLoopThread::PostWork()`.
    */
    std::shared_ptr<Disposable> ScheduleWork(Closure work) override;

    /**
    @copydoc zaf::rx::Scheduler::ScheduleDelayedWork

    ---
    @throw zaf::rx::ExecutionStoppedError
        The scheduler is stopped. This may occur if the scheduler is being destructed while an
        executing work tries to schedule a new work.

    @throw ...
        Any exception thrown by the underlying thread implementation.

    @details
        This method delegates to `zaf::rx::RunLoopThread::PostDelayedWork()`.
    */
    std::shared_ptr<Disposable> ScheduleDelayedWork(
        std::chrono::steady_clock::duration delay, 
        Closure work) override;

private:
    std::shared_ptr<RunLoopThread> thread_;
};

}