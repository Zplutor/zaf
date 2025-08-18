#pragma once

/**
@file
    Defines the `zaf::rx::DefaultRunLoopThread` class.
*/

#include <deque>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>
#include <zaf/rx/internal/thread/thread_work_item_base.h>
#include <zaf/rx/thread/run_loop_thread.h>

namespace zaf::testing {
class DefaultRunLoopThreadTest;
}

namespace zaf::rx {

/**
Represents a run loop thread with a default implementation.
*/
class DefaultRunLoopThread : public RunLoopThread {
public:
    /**
    Constructs the instance and starts the thread.

    @throw std::bad_alloc
    @throw std::system_error
        Thrown if the thread cannot be created.
    */
    DefaultRunLoopThread();

    /**
    Destructs the instance and stops the thread.

    @details
        This method waits for the thread to finish executing all queued works before returning.
    */
    ~DefaultRunLoopThread();

    /**
    @copydoc zaf::rx::RunLoopThread::PostWork()

    ---
    @throw std::bad_alloc
    @throw zaf::InvalidOperationError
        Thrown if the thread is stopped.
    */
    std::shared_ptr<Disposable> PostWork(Closure work) override;

    /**
    @copydoc zaf::rx::RunLoopThread::PostDelayedWork()

    ---
    @throw std::bad_alloc
    @throw zaf::InvalidOperationError
        Thrown if the thread is stopped.
    */
    std::shared_ptr<Disposable> PostDelayedWork(
        std::chrono::steady_clock::duration delay, 
        Closure work) override;

private:
    class WorkItem;
    class DelayedWorkItem;

    class State {
    public:
        std::mutex lock;
        std::atomic<bool> is_stopped{};
        std::condition_variable work_event;

        // Works that need to be executed without delay.
        std::vector<std::shared_ptr<WorkItem>> queued_work_items;

        // Works that need to be executed after a delay.
        // Sorted by execute time point.
        std::deque<std::shared_ptr<DelayedWorkItem>> delayed_work_items;
    };

    class WorkItem : public internal::ThreadWorkItemBase {
    public:
        explicit WorkItem(Closure work) noexcept : internal::ThreadWorkItemBase(std::move(work)) {

        }

    protected:
        void OnDispose() noexcept override {
            // No additional cleanup is needed for this work item.
        }
    };

    class DelayedWorkItem : public WorkItem {
    public:
        DelayedWorkItem(
            std::chrono::steady_clock::time_point execute_time_point,
            Closure work,
            std::weak_ptr<State> state) noexcept;

        const std::chrono::steady_clock::time_point& ExecuteTimePoint() const noexcept {
            return execute_time_point_;
        }

    protected:
        void OnDispose() noexcept override;

    private:
        std::chrono::steady_clock::time_point execute_time_point_{};
        std::weak_ptr<State> state_;
    };

private:
    static void ThreadProcedure(const std::shared_ptr<State>& state);
    static std::optional<std::chrono::steady_clock::duration> ProcessDueDelayedWorkItems(
        const std::shared_ptr<State>& state);
    static void ExecuteQueuedWorkItems(const std::vector<std::shared_ptr<WorkItem>>& work_items);

private:
    friend class zaf::testing::DefaultRunLoopThreadTest;

    std::shared_ptr<Disposable> PostWorkAt(
        std::chrono::steady_clock::time_point execute_time_point, 
        Closure work);

private:
    std::shared_ptr<State> state_;
    std::thread thread_;
};

}