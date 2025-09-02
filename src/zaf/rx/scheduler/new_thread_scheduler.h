#pragma once

/**
@file
    Defines the `zaf::rx::NewThreadScheduler` class.
*/

#include <mutex>
#include <unordered_set>
#include <zaf/rx/internal/thread/thread_work_item_base.h>
#include <zaf/rx/scheduler/scheduler.h>
#include <zaf/rx/thread/run_loop_thread.h>

namespace zaf::testing {
class NewThreadSchedulerTest;
}

namespace zaf::rx {

/**
Represents a scheduler that uses a new thread to execute each individual work.
*/
class NewThreadScheduler : public Scheduler {
public:
    /**
    Gets the default instance of `zaf::rx::NewThreadScheduler`.

    @return
        The singleton of the default instance.

    @post
        The returned object is not null.

    @throw std::bad_alloc
        Thrown if fails to create the instance.
    */
    static const std::shared_ptr<NewThreadScheduler>& Default();

public:
    /**
    Constructs an instance.

    @details
        Typically, it is unnecessary to create multiple instances of this scheduler. Use the 
        `Default()` method the get the default instance instead.
    */
    NewThreadScheduler();

    /**
    Destructors the instance.

    @details
        This method will block until all threads created by this scheduler finish their works and 
        terminate.
    */
    ~NewThreadScheduler();

    /**
    @copydoc zaf::rx::Scheduler::ScheduleWork

    ---
    @pre
        The scheduler is not stopped.

    @throw zaf::PreconditionError
        Thrown if the scheduler is stopped. This may occur if the scheduler is being destructed 
        while an executing work tries to schedule a new work.

    @throw std::bad_alloc

    @throw std::system_error
        Thrown if fails to create a new thread.

    @throw ...
        Any exception thrown by the underlying thread implementation.

    @details
        This method creates a new thread to execute the specified work.
    */
    std::shared_ptr<Disposable> ScheduleWork(Closure work) override;

    /**
    @copydoc zaf::rx::Scheduler::ScheduleDelayedWork

    ---
    @pre
        The scheduler is not stopped.

    @throw zaf::PreconditionError
        Thrown if the scheduler is stopped. This may occur if the scheduler is being destructed
        while an executing work tries to schedule a new work.

    @throw std::bad_alloc

    @throw std::system_error
        Thrown if fails to create a new thread.

    @throw ...
        Any exception thrown by the underlying thread implementation.

    @details
        This method creates a new thread to execute the specified work.
    */
    std::shared_ptr<Disposable> ScheduleDelayedWork(
        std::chrono::steady_clock::duration delay,
        Closure work) override;

private:
    class WorkItem;

    class SharedState {
    public:
        std::mutex mutex;
        std::unordered_set<std::shared_ptr<WorkItem>> work_items;
        bool is_stopped{};
    };

    class WorkItem :
        public internal::ThreadWorkItemBase,
        public std::enable_shared_from_this<WorkItem> {

    public:
        WorkItem(Closure work, std::weak_ptr<SharedState>);
        ~WorkItem();

        void RunOnThread(std::optional<std::chrono::steady_clock::duration> delay);
        void JoinThread();

    protected:
        void OnDispose() noexcept override;

    private:
        std::weak_ptr<SharedState> shared_state_;
        std::unique_ptr<RunLoopThread> thread_;
        std::shared_ptr<Disposable> work_disposable_;
    };

private:
    std::shared_ptr<Disposable> ScheduleWorkOnThread(
        Closure work,
        std::optional<std::chrono::steady_clock::duration> delay);

    std::size_t ThreadCount() const noexcept;

private:
    std::shared_ptr<SharedState> shared_state_;

    friend class testing::NewThreadSchedulerTest;
};

}