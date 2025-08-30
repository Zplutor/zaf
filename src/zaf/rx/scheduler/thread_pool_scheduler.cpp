#include <zaf/rx/scheduler/thread_pool_scheduler.h>
#include <zaf/base/error/contract_error.h>
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
        std::lock_guard lock(shared_state_->queued_work_items_mutex);
        shared_state_->is_stopped = true;
    }
    shared_state_->queued_work_items_cv.notify_all();

    std::vector<std::unique_ptr<RunLoopThread>> threads;
    {
        std::lock_guard lock(shared_state_->threads_mutex);
        threads = std::move(shared_state_->threads);
    }
    // Wait for all threads to exit.
    threads.clear();
}


void ThreadPoolScheduler::Initialize(std::size_t max_thread_count) {
    shared_state_ = std::make_shared<SharedState>();
    shared_state_->max_thread_count = max_thread_count;
}


void ThreadPoolScheduler::CreateFirstThreadIfNeeded() {

    if (shared_state_->total_thread_count > 0) {
        return;
    }

    std::lock_guard lock(shared_state_->threads_mutex);
    if (shared_state_->total_thread_count > 0) {
        return;
    }

    CreateThreadInLock(shared_state_);
}


void ThreadPoolScheduler::CreateTimerThreadIfNeeded() {
    std::call_once(shared_state_->timer_thread_init_flag, [this]() {
        shared_state_->timer_thread = std::make_unique<DefaultRunLoopThread>();
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
    ScheduleWorkItem(shared_state_, work_item);
    return work_item;
}


std::shared_ptr<Disposable> ThreadPoolScheduler::ScheduleDelayedWork(
    std::chrono::steady_clock::duration delay, 
    Closure work) {

    ZAF_EXPECT(work);

    // Same as ScheduleWork, ensure at least one thread in the pool.
    CreateFirstThreadIfNeeded();
    CreateTimerThreadIfNeeded();
    
    auto work_item = std::make_shared<DelayedWorkItem>(std::move(work));
    auto delay_disposable = shared_state_->timer_thread->PostDelayedWork(
        delay,
        std::bind(&ThreadPoolScheduler::ScheduleWorkItem, shared_state_, work_item));

    work_item->SetDelayDisposable(std::move(delay_disposable));
    return work_item;
}


void ThreadPoolScheduler::ScheduleWorkItem(
    const std::shared_ptr<SharedState>& shared_state,
    std::shared_ptr<WorkItem> work_item) {

    bool need_create_thread{};
    {
        std::lock_guard lock(shared_state->queued_work_items_mutex);
        shared_state->queued_work_items.push_back(std::move(work_item));

        // If the queued work items are more than 1, it is considered that there is no free thread.
        // So we need to create a new thread if possible.
        need_create_thread = shared_state->queued_work_items.size() > 1;
    }
    shared_state->queued_work_items_cv.notify_one();

    if (need_create_thread) {
        // Try to create a new thread if possible. If failed, there is still at least one thread in
        // the pool to process the work item.
        TryCreateNewThread(shared_state);
    }
}


void ThreadPoolScheduler::TryCreateNewThread(
    const std::shared_ptr<SharedState>& shared_state) noexcept {

    if (shared_state->total_thread_count >= shared_state->max_thread_count) {
        return;
    }

    std::lock_guard lock(shared_state->threads_mutex);
    if (shared_state->total_thread_count >= shared_state->max_thread_count) {
        return;
    }

    try {
        CreateThreadInLock(shared_state);
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


void ThreadPoolScheduler::ThreadProcedure(const std::shared_ptr<SharedState>& shared_state) {

    std::unique_lock<std::mutex> lock(shared_state->queued_work_items_mutex, std::defer_lock);
    while (true) {

        std::shared_ptr<WorkItem> work_item;

        lock.lock();

        if (shared_state->queued_work_items.empty()) {
            shared_state->queued_work_items_cv.wait(lock, [&shared_state]() {
                return !shared_state->queued_work_items.empty() || shared_state->is_stopped;
            });
            // Exit only when there is no work.
            if (shared_state->is_stopped) {
                break;
            }
        }

        work_item = std::move(shared_state->queued_work_items.front());
        shared_state->queued_work_items.pop_front();

        lock.unlock();

        if (work_item) {
            work_item->RunWork();
        }
    }
}

}