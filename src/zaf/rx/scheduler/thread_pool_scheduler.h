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
        return shared_state_->total_thread_count;
    }

    /**
    Gets the maximum number of threads that can be created in the thread pool.
    */
    std::size_t MaxThreadCount() const noexcept {
        return shared_state_->max_thread_count;
    }

    /**
    @copydoc zaf::rx::Scheduler::ScheduleWork()

    ---
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

        std::deque<std::shared_ptr<WorkItem>> queued_work_items;
        std::mutex queued_work_items_mutex;
        std::condition_variable queued_work_items_cv;
        
        // This variables should be accessed only when holding queued_work_items_mutex.
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
        explicit DelayedWorkItem(Closure work) : WorkItem(std::move(work)) {

        }

        void SetDelayDisposable(std::shared_ptr<Disposable> disposable) {
            delay_disposable_ = std::move(disposable);
        }

        ~DelayedWorkItem() {
            DoDisposal();
        }

    protected:
        void OnDispose() noexcept override {
            DoDisposal();
        }

    private:
        void DoDisposal() {
            if (delay_disposable_) {
                delay_disposable_->Dispose();
                delay_disposable_.reset();
            }
        }

    private:
        std::shared_ptr<Disposable> delay_disposable_;
    };

private:
    void Initialize(std::size_t max_thread_count);
    void CreateFirstThreadIfNeeded();
    void CreateTimerThreadIfNeeded();

    static void ScheduleWorkItem(
        const std::shared_ptr<SharedState>& shared_state,
        std::shared_ptr<WorkItem> work_item);
    
    static void TryCreateNewThread(const std::shared_ptr<SharedState>& shared_state) noexcept;
    static void CreateThreadInLock(const std::shared_ptr<SharedState>& shared_state);

    static void ThreadProcedure(const std::shared_ptr<SharedState>& shared_state);

private:
    std::shared_ptr<SharedState> shared_state_;
};

}