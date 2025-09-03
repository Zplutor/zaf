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

@details
    The default run loop thread is designed for general-purpose use. It executes one work per run 
    loop iteration, and checks for due delayed works per run loop iteration as well.
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
    @throw zaf::rx::ExecutionStoppedError
        Thrown if the thread is stopped. This may occur if the thread is being destructed while an 
        executing work tries to post a new work.

    @throw std::bad_alloc
    */
    std::shared_ptr<Disposable> PostWork(Closure work) override;

    /**
    @copydoc zaf::rx::RunLoopThread::PostDelayedWork()

    ---
    @throw zaf::rx::ExecutionStoppedError
        Thrown if the thread is stopped. This may occur if the thread is being destructed while an 
        executing work tries to post a new work.

    @throw std::bad_alloc
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

        // A hybrid queue that contains both immediate works and delayed works.
        // Immediate works are always in the front of the queue, sorting by their posting order.
        // Delayed works are always in the back of the queue, sorting by their execute time point.
        std::deque<std::shared_ptr<WorkItem>> hybrid_queue;

        // Count of immediate works in the hybrid queue.
        std::size_t immediate_work_count{};
    };

    class WorkItem : public internal::ThreadWorkItemBase {
    public:
        explicit WorkItem(Closure work) noexcept : internal::ThreadWorkItemBase(std::move(work)) {

        }

        const std::chrono::steady_clock::time_point& ExecuteTimePoint() const noexcept {
            return execute_time_point_;
        }

    protected:
        WorkItem(
            Closure work,
            std::chrono::steady_clock::time_point execute_time_point) 
            :
            internal::ThreadWorkItemBase(std::move(work)),
            execute_time_point_(execute_time_point) {

        }

        void OnDispose() noexcept override {
            // No additional cleanup is needed for this work item.
        }

    private:
        std::chrono::steady_clock::time_point execute_time_point_{};
    };

    class DelayedWorkItem : public WorkItem {
    public:
        DelayedWorkItem(
            std::chrono::steady_clock::time_point execute_time_point,
            Closure work,
            std::weak_ptr<State> state) noexcept;

    protected:
        void OnDispose() noexcept override;

    private:
        void DoWork(const Closure& work);

    private:
        std::weak_ptr<State> state_;
    };

private:
    static void ThreadProcedure(const std::shared_ptr<State>& state);
    static std::optional<std::chrono::steady_clock::duration> ProcessDueDelayedWorkItems(
        const std::shared_ptr<State>& state) noexcept;

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