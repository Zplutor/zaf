#include <zaf/rx/thread/default_run_loop_thread.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/rx/execution_stopped_error.h>

namespace zaf::rx {

DefaultRunLoopThread::DefaultRunLoopThread() : 
    state_(std::make_shared<State>()),
    thread_(std::bind(&DefaultRunLoopThread::ThreadProcedure, state_)) {

}


DefaultRunLoopThread::~DefaultRunLoopThread() {

    {
        std::lock_guard<std::mutex> lock(state_->lock);
        state_->is_stopped.store(true);
    }
    state_->work_event.notify_one();

    if (std::this_thread::get_id() == thread_.get_id()) {
        thread_.detach();
    }
    else {
        thread_.join();
    }
}


std::shared_ptr<Disposable> DefaultRunLoopThread::PostWork(Closure work) {

    ZAF_EXPECT(work);

    std::shared_ptr<WorkItem> work_item;
    {
        std::lock_guard<std::mutex> lock(state_->lock);
        if (state_->is_stopped) {
            throw ExecutionStoppedError(ZAF_SOURCE_LOCATION());
        }

        work_item = std::make_shared<WorkItem>(std::move(work));

        auto insert_position = state_->hybrid_queue.begin() + state_->immediate_work_count;
        state_->hybrid_queue.insert(insert_position, work_item);
        state_->immediate_work_count++;
    }

    state_->work_event.notify_one();
    return work_item;
}


std::shared_ptr<Disposable> DefaultRunLoopThread::PostDelayedWork(
    std::chrono::steady_clock::duration delay, 
    Closure work) {

    ZAF_EXPECT(work);

    return PostWorkAt(std::chrono::steady_clock::now() + delay, std::move(work));
}


std::shared_ptr<Disposable> DefaultRunLoopThread::PostWorkAt(
    std::chrono::steady_clock::time_point execute_time_point, 
    Closure work) {

    auto work_item = std::make_shared<DelayedWorkItem>(
        execute_time_point,
        std::move(work),
        state_);

    {
        std::lock_guard<std::mutex> lock(state_->lock);
        if (state_->is_stopped) {
            throw ExecutionStoppedError(ZAF_SOURCE_LOCATION());
        }

        auto iterator = std::lower_bound(
            state_->hybrid_queue.begin() + state_->immediate_work_count,
            state_->hybrid_queue.end(),
            work_item,
            [](const std::shared_ptr<WorkItem>& existing,
               const std::shared_ptr<DelayedWorkItem>& new_time) {
                return existing->ExecuteTimePoint() < new_time->ExecuteTimePoint();
            });

        // The new work item should be inserted after the item which has the same execute time 
        // point. 
        if ((iterator != state_->hybrid_queue.end()) &&
            ((*iterator)->ExecuteTimePoint() == work_item->ExecuteTimePoint())) {

            state_->hybrid_queue.insert(iterator + 1, work_item);
        }
        else {
            state_->hybrid_queue.insert(iterator, work_item);
        }
    }

    state_->work_event.notify_one();
    return work_item;
}


void DefaultRunLoopThread::ThreadProcedure(const std::shared_ptr<State>& state) {

    std::unique_lock<std::mutex> lock(state->lock, std::defer_lock);
    while (true) {

        lock.lock();
        if (state->is_stopped) {
            break;
        }

        auto wait_duration = ProcessDueDelayedWorkItems(state);

        if (state->immediate_work_count == 0) {
            if (wait_duration) {
                state->work_event.wait_for(lock, *wait_duration);
            }
            else {
                state->work_event.wait(lock);
            }
        }

        // Execute one immediate work per run loop iteration.
        std::shared_ptr<WorkItem> work_item;
        if (state->immediate_work_count > 0) {

            work_item = std::move(state->hybrid_queue.front());
            state->hybrid_queue.pop_front();
            state->immediate_work_count--;
        }
        lock.unlock();

        if (work_item) {
            work_item->RunWork();
        }
    }

    // All queued works should be executed before the thread exits.
    // The lock is already held here, so no need to lock again.
    ProcessDueDelayedWorkItems(state);
    auto remain_work_items = std::move(state->hybrid_queue);
    auto immediate_work_count = state->immediate_work_count;
    lock.unlock();

    auto end_iterator = remain_work_items.begin() + immediate_work_count;
    for (auto iterator = remain_work_items.begin(); iterator != end_iterator; ++iterator) {
        (*iterator)->RunWork();
    }
}


std::optional<std::chrono::steady_clock::duration> 
    DefaultRunLoopThread::ProcessDueDelayedWorkItems(
        const std::shared_ptr<State>& state) noexcept {

    if (state->hybrid_queue.empty()) {
        return std::nullopt;
    }

    // Move due delayed works to immediate works before executing works, to avoid delayed work 
    // starvation.
    auto now = std::chrono::steady_clock::now();

    for (auto iterator = state->hybrid_queue.begin() + state->immediate_work_count;
         iterator != state->hybrid_queue.end();
         ++iterator) {

        if ((*iterator)->ExecuteTimePoint() <= now) {
            state->immediate_work_count++;
        }
        else {
            return (*iterator)->ExecuteTimePoint() - now;
        }
    }

    return std::nullopt;
}


DefaultRunLoopThread::DelayedWorkItem::DelayedWorkItem(
    std::chrono::steady_clock::time_point execute_time_point,
    Closure work,
    std::weak_ptr<State> state) noexcept
    :
    WorkItem(std::bind(&DelayedWorkItem::DoWork, this, std::move(work)), execute_time_point),
    state_(std::move(state)) {

}


void DefaultRunLoopThread::DelayedWorkItem::DoWork(const Closure& work) {

    work();

    // The base class would call Dispose() after this method returns, and OnDispose() would be 
    // called as well. We reset the state_ here to avoid unnecessary disposal work in OnDispose().
    state_.reset();
}


void DefaultRunLoopThread::DelayedWorkItem::OnDispose() noexcept {

    auto state = state_.lock();
    if (!state) {
        return;
    }

    bool need_notify{};
    {
        std::lock_guard<std::mutex> lock(state->lock);

        auto delayed_begin = state->hybrid_queue.begin() + state->immediate_work_count;
        auto iterator = std::lower_bound(
            delayed_begin,
            state->hybrid_queue.end(),
            this,
            [](const std::shared_ptr<WorkItem>& existing,
               DelayedWorkItem* current_item) {
                return existing->ExecuteTimePoint() < current_item->ExecuteTimePoint();
            });

        for (; iterator != state->hybrid_queue.end(); ++iterator) {

            if ((*iterator)->ExecuteTimePoint() != this->ExecuteTimePoint()) {
                break;
            }

            if ((*iterator).get() == this) {
                if (iterator == delayed_begin) {
                    need_notify = true;
                }
                state->hybrid_queue.erase(iterator);
                break;
            }
        }
    }

    if (need_notify) {
        state->work_event.notify_one();
    }
}

}