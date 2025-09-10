#include <zaf/rx/scheduler/thread_pool_scheduler.h>
#include <zaf/base/error/precondition_error.h>
#include <zaf/rx/execution_stopped_error.h>
#include <zaf/rx/thread/default_run_loop_thread.h>

namespace zaf::rx {

const std::shared_ptr<ThreadPoolScheduler>& ThreadPoolScheduler::Default() {
    static auto default_instance = std::make_shared<ThreadPoolScheduler>();
    return default_instance;
}


ThreadPoolScheduler::ThreadPoolScheduler() {
    auto max_thread_count = std::thread::hardware_concurrency();
    if (max_thread_count == 0) {
        max_thread_count = 4;
    }
    Initialize(max_thread_count);
}


ThreadPoolScheduler::ThreadPoolScheduler(std::size_t max_thread_count) {
    ZAF_EXPECT(max_thread_count > 0);
    Initialize(max_thread_count);
}


ThreadPoolScheduler::~ThreadPoolScheduler() {
    {
        std::lock_guard lock(state_->hybrid_queue_mutex);
        state_->is_stopped = true;
    }
    state_->hybrid_queue_cv.notify_all();

    std::vector<std::unique_ptr<RunLoopThread>> threads;
    {
        std::lock_guard lock(state_->threads_mutex);
        threads = std::move(state_->threads);
    }
    // Wait for all threads to exit.
    threads.clear();
}


void ThreadPoolScheduler::Initialize(std::size_t max_thread_count) {
    state_ = std::make_shared<SharedState>();
    state_->max_thread_count = max_thread_count;
}


void ThreadPoolScheduler::CreateFirstThreadIfNeeded() {

    if (state_->total_thread_count > 0) {
        return;
    }

    std::lock_guard lock(state_->threads_mutex);
    if (state_->total_thread_count > 0) {
        return;
    }

    CreateThreadInLock(state_);
}


void ThreadPoolScheduler::CreateTimerThreadIfNeeded() {
    std::call_once(state_->timer_thread_init_flag, [this]() {
        state_->timer_thread = std::make_unique<DefaultRunLoopThread>();
    });
}


std::shared_ptr<Disposable> ThreadPoolScheduler::ScheduleWork(Closure work) {

    ZAF_EXPECT(work);

    // Ensure at least one thread in the pool, otherwise the work may not be executed if thread 
    // creation is failed after the work item is queued. 
    // If the first thread creation is failed, exception will be thrown and the work item won't be 
    // queued, which is expected.
    CreateFirstThreadIfNeeded();

    auto work_item = std::make_shared<WorkItem>(std::move(work));

    bool need_create_thread{};
    {
        std::lock_guard lock(state_->hybrid_queue_mutex);
        if (state_->is_stopped) {
            throw ExecutionStoppedError(ZAF_SOURCE_LOCATION());
        }

        auto insert_position = state_->hybrid_queue.begin() + state_->immediate_work_count;
        state_->hybrid_queue.insert(insert_position, work_item);
        state_->immediate_work_count++;

        // If the immediate work items are more than 1, it is considered that there is no free 
        // thread. So we need to create a new thread if possible.
        need_create_thread = state_->immediate_work_count > 1;
    }
    state_->hybrid_queue_cv.notify_one();

    if (need_create_thread) {
        // Try to create a new thread if possible. If failed, there is still at least one thread in
        // the pool to process the work item.
        TryCreateNewThread(state_);
    }
    return work_item;
}


std::shared_ptr<Disposable> ThreadPoolScheduler::ScheduleDelayedWork(
    std::chrono::steady_clock::duration delay, 
    Closure work) {

    ZAF_EXPECT(work);

    // Same as ScheduleWork, ensure at least one thread in the pool.
    CreateFirstThreadIfNeeded();
    CreateTimerThreadIfNeeded();
    
    auto work_item = std::make_shared<DelayedWorkItem>(std::move(work), state_);

    // Insert the delayed work item into the hybrid queue.
    {
        std::lock_guard lock(state_->hybrid_queue_mutex);
        if (state_->is_stopped) {
            throw ExecutionStoppedError(ZAF_SOURCE_LOCATION());
        }

        auto insert_position = std::lower_bound(
            state_->hybrid_queue.begin() + state_->immediate_work_count,
            state_->hybrid_queue.end(),
            work_item);

        state_->hybrid_queue.insert(insert_position, work_item);
    }

    auto delay_disposable = state_->timer_thread->PostDelayedWork(
        delay,
        std::bind(&ThreadPoolScheduler::OnDelayedWorkItemReady, state_, work_item));

    work_item->SetDelayDisposable(std::move(delay_disposable));
    return work_item;
}


void ThreadPoolScheduler::OnDelayedWorkItemReady(
    const std::weak_ptr<SharedState>& weak_state,
    const std::shared_ptr<WorkItem>& work_item) noexcept {

    auto state = weak_state.lock();
    if (!state) {
        return;
    }

    // Move the due delayed work item to the immediate work items in the hybrid queue.
    bool need_create_thread{};
    {
        std::lock_guard lock(state->hybrid_queue_mutex);

        auto iterator = std::lower_bound(
            state->hybrid_queue.begin() + state->immediate_work_count,
            state->hybrid_queue.end(),
            work_item);

        if (iterator == state->hybrid_queue.end() || (*iterator) != work_item) {
            // The work item is not in the queue, probably disposed.
            return;
        }

        std::rotate(
            state->hybrid_queue.begin() + state->immediate_work_count,
            iterator,
            iterator + 1);

        state->immediate_work_count++;

        // Same as ScheduleWork.
        need_create_thread = state->immediate_work_count > 1;
    }

    state->hybrid_queue_cv.notify_one();

    if (need_create_thread) {
        TryCreateNewThread(state);
    }
}


void ThreadPoolScheduler::TryCreateNewThread(const std::shared_ptr<SharedState>& state) noexcept {

    if (state->total_thread_count >= state->max_thread_count) {
        return;
    }

    std::lock_guard lock(state->threads_mutex);
    if (state->total_thread_count >= state->max_thread_count) {
        return;
    }

    try {
        CreateThreadInLock(state);
    }
    catch (...) {
        // Ignore all exceptions.
    }
}


void ThreadPoolScheduler::CreateThreadInLock(const std::shared_ptr<SharedState>& shared_state) {

    // Allocate space in advance to ensure that adding a new thread won't throw.
    // We don't reserve(max_thread_count) as it may be a large number.
    shared_state->threads.reserve(shared_state->threads.size() + 1);

    auto thread = std::make_unique<DefaultRunLoopThread>();
    thread->PostWork(std::bind(&ThreadPoolScheduler::ThreadProcedure, shared_state));

    shared_state->threads.push_back(std::move(thread));
    shared_state->total_thread_count++;
}


void ThreadPoolScheduler::ThreadProcedure(const std::shared_ptr<SharedState>& state) {

    std::unique_lock<std::mutex> lock(state->hybrid_queue_mutex, std::defer_lock);
    while (true) {

        lock.lock();

        if (state->immediate_work_count == 0) {
            state->hybrid_queue_cv.wait(lock, [&state]() {
                return (state->immediate_work_count > 0) || state->is_stopped;
            });
            // Exit only when there is no work.
            if (state->is_stopped) {
                break;
            }
        }

        auto work_item = std::move(state->hybrid_queue.front());
        state->hybrid_queue.pop_front();
        state->immediate_work_count--;

        lock.unlock();

        if (work_item) {
            work_item->RunWork();
        }
    }
}


std::size_t ThreadPoolScheduler::HybridQueueSize() const noexcept {
    std::lock_guard<std::mutex> lock(state_->hybrid_queue_mutex);
    return state_->hybrid_queue.size();
}


ThreadPoolScheduler::DelayedWorkItem::DelayedWorkItem(
    Closure work, 
    std::weak_ptr<SharedState> state)
    :
    WorkItem(std::move(work)),
    state_(std::move(state)),
    delay_disposable_(Disposable::Empty()) {

}


/*
The delay_disposable_ may be called on multiple threads:
1. The thread calls ScheduleDelayedWork to set the delay disposable.
2. The thread in the pool that executes and disposes the work item.
3. The thread that calls Dispose on the returned disposable.
So delay_disposable_ must be accessed in a thread-safe way. See the TimerProducer for more details.
*/
void ThreadPoolScheduler::DelayedWorkItem::SetDelayDisposable(
    const std::shared_ptr<Disposable>& disposable) noexcept {

    auto expected = Disposable::Empty();
    if (!delay_disposable_.compare_exchange_strong(expected, disposable)) {
        disposable->Dispose();
    }
}


ThreadPoolScheduler::DelayedWorkItem::~DelayedWorkItem() {
    DisposeDelay();
}


void ThreadPoolScheduler::DelayedWorkItem::OnDispose() noexcept {
    DisposeDelay();
    RemoveFromQueue();
}


void ThreadPoolScheduler::DelayedWorkItem::DisposeDelay() noexcept {

    auto disposable = delay_disposable_.exchange(nullptr);
    if (disposable) {
        disposable->Dispose();
    }
}


void ThreadPoolScheduler::DelayedWorkItem::RemoveFromQueue() noexcept {

    auto state = state_.lock();
    if (!state) {
        return;
    }

    std::lock_guard<std::mutex> lock(state->hybrid_queue_mutex);

    auto iterator = std::lower_bound(
        state->hybrid_queue.begin() + state->immediate_work_count,
        state->hybrid_queue.end(),
        this,
        [](const std::shared_ptr<WorkItem>& existing, DelayedWorkItem* current) {
            return existing.get() < current;
        });

    if (iterator == state->hybrid_queue.end() || (*iterator).get() != this) {
        return;
    }

    state->hybrid_queue.erase(iterator);
}

}