#pragma once

/**
@file
    Defines the `zaf::rx::ThreadPoolScheduler` class.
*/

#include <deque>
#include <mutex>
#include <vector>
#include <zaf/rx/internal/thread/thread_work_item_base.h>
#include <zaf/rx/scheduler/scheduler.h>
#include <zaf/rx/thread/run_loop_thread.h>

namespace zaf::testing {
class ThreadPoolSchedulerTest;
}

namespace zaf::rx {

/**
Represents a scheduler that uses a pool of threads to execute work items.

@details
    The thread pool will create threads on demand when there is no free thread to execute work 
    items. Created threads won't be terminated until the scheduler is destructed.

    An individual timer thread is used to handle delayed work items. Delayed work items are queued
    to the thread pool only when their delay time is reached.
*/
class ThreadPoolScheduler : public Scheduler {
public:
    /**
    Gets the default instance of `zaf::rx::ThreadPoolScheduler`.

    @return
        The default singleton instance.

    @throw std::bad_alloc

    @details
        The default instance is created with the default constructor.

    @see `zaf::rx::ThreadPoolScheduler::ThreadPoolScheduler()`
    */
    static const std::shared_ptr<ThreadPoolScheduler>& Default();

public:
    /**
    Constructs the instance with a maximum thread count equal to the number of concurrent threads 
    supported by the hardware.

    @throw std::bad_alloc

    @details
        This constructor uses `std::thread::hardware_concurrency()` to get the number of concurrent
        threads supported by the hardware. If this number cannot be determined, the maximum thread 
        count will be set to 4.
    */
    ThreadPoolScheduler();

    /**
    Constructs the instance with the specified maximum thread count.

    @param max_thread_count
        The maximum number of threads that can be created in the thread pool.

    @pre 
        `max_thread_count` must be greater than 0.

    @throw zaf::PreconditionError
    @throw std::bad_alloc
    */
    explicit ThreadPoolScheduler(std::size_t max_thread_count);

    /**
    Destructs the instance, stopping all threads in the thread pool.

    @details
        This method waits for all threads to finish executing all queued works before returning.
    */
    ~ThreadPoolScheduler();

    /**
    Gets the number of currently existing threads in the thread pool.
    */
    std::size_t CurrentThreadCount() const noexcept {
        return state_->total_thread_count;
    }

    /**
    Gets the maximum number of threads that can be created in the thread pool.
    */
    std::size_t MaxThreadCount() const noexcept {
        return state_->max_thread_count;
    }

    /**
    @copydoc zaf::rx::Scheduler::ScheduleWork()

    ---
    @throw zaf::rx::ExecutionStoppedError
        Thrown if scheduler is stopped. This may occur if the scheduler is being destructed while 
        an executing work tries to schedule a new work.

    @throw std::bad_alloc

    @throw std::system_error
        Thrown if the first thread in the thread pool cannot be created. Failing to create threads
        other than the first one won't throw exception, as there is at least one thread in the pool
        to execute the work.

    @throw ...
        Any exception thrown by the underlying thread implementation when initializing the first
        thread.
    */
    std::shared_ptr<Disposable> ScheduleWork(Closure work) override;

    /**
    @copydoc zaf::rx::Scheduler::ScheduleDelayedWork()

    ---
    @throw zaf::rx::ExecutionStoppedError
        Thrown if scheduler is stopped. This may occur if the scheduler is being destructed while
        an executing work tries to schedule a new work.

    @throw std::bad_alloc

    @throw std::system_error
        Thrown if the first thread in the thread pool or the timer thread cannot be created.

    @throw ...
        Any exception thrown by the underlying thread implementation when scheduling a delay in the
        timer thread.
    */
    std::shared_ptr<Disposable> ScheduleDelayedWork(
        std::chrono::steady_clock::duration delay,
        Closure work) override;

private:
    class WorkItem;

    class SharedState {
    public:
        std::size_t max_thread_count{};

        // A queue that contains both immediate work items and delayed work items.
        // Immediate work items are always in the front of the queue, ordered by their scheduling 
        // order. Delayed work items are always in the back of the queue, ordered by their pointer
        // value.
        std::deque<std::shared_ptr<WorkItem>> hybrid_queue;

        // Count of immediate work items in the hybrid queue.
        std::size_t immediate_work_count{};

        std::mutex hybrid_queue_mutex;
        std::condition_variable hybrid_queue_cv;
        
        // This variables should be accessed only when holding hybrid_queue_mutex.
        bool is_stopped{ false };

        std::vector<std::unique_ptr<RunLoopThread>> threads;
        std::mutex threads_mutex;
        // Extra atomic to avoid locking when checking thread counts.
        std::atomic<std::size_t> total_thread_count{};

        // Individual timer thread to handle delayed work items.
        std::unique_ptr<RunLoopThread> timer_thread;
        std::once_flag timer_thread_init_flag;
    };

    class WorkItem : public internal::ThreadWorkItemBase {
    public:
        explicit WorkItem(Closure work) : ThreadWorkItemBase(std::move(work)) {

        }

    protected:
        void OnDispose() noexcept override {
            // Nothing to do.
        }
    };

    class DelayedWorkItem : public WorkItem {
    public:
        DelayedWorkItem(Closure work, std::weak_ptr<SharedState> state);

        void SetDelayDisposable(const std::shared_ptr<Disposable>& disposable) noexcept;

        ~DelayedWorkItem();

    protected:
        void OnDispose() noexcept override;

    private:
        void DisposeDelay() noexcept;
        void RemoveFromQueue() noexcept;

    private:
        std::weak_ptr<SharedState> state_;
        std::atomic<std::shared_ptr<Disposable>> delay_disposable_;
    };

private:
    void Initialize(std::size_t max_thread_count);
    void CreateFirstThreadIfNeeded();
    void CreateTimerThreadIfNeeded();

    static void OnDelayedWorkItemReady(
        const std::weak_ptr<SharedState>& weak_state,
        const std::shared_ptr<WorkItem>& work_item) noexcept;

    static void TryCreateNewThread(const std::shared_ptr<SharedState>& state) noexcept;
    static void CreateThreadInLock(const std::shared_ptr<SharedState>& state);

    static void ThreadProcedure(const std::shared_ptr<SharedState>& state);

    std::size_t HybridQueueSize() const noexcept;

private:
    std::shared_ptr<SharedState> state_;

    friend class zaf::testing::ThreadPoolSchedulerTest;
};

}