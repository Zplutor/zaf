#include <zaf/rx/thread/default_run_loop_thread.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/base/error/invalid_operation_error.h>

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
            throw InvalidOperationError(ZAF_SOURCE_LOCATION());
        }

        work_item = std::make_shared<WorkItem>(std::move(work));
        state_->queued_work_items.push_back(work_item);
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
            throw InvalidOperationError(ZAF_SOURCE_LOCATION());
        }

        auto iterator = std::lower_bound(
            state_->delayed_work_items.begin(),
            state_->delayed_work_items.end(),
            work_item,
            [](const std::shared_ptr<DelayedWorkItem>& existing,
               const std::shared_ptr<DelayedWorkItem>& new_time) {
                return existing->ExecuteTimePoint() < new_time->ExecuteTimePoint();
            });

        // The new work item should be inserted after the item which has the same execute time 
        // point. 
        if ((iterator != state_->delayed_work_items.end()) &&
            ((*iterator)->ExecuteTimePoint() == work_item->ExecuteTimePoint())) {

            state_->delayed_work_items.insert(std::next(iterator), work_item);
        }
        else {
            state_->delayed_work_items.insert(iterator, work_item);
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

        if (state->queued_work_items.empty()) {
            if (wait_duration) {
                state->work_event.wait_for(lock, *wait_duration);
            }
            else {
                state->work_event.wait(lock);
            }
        }

        auto executed_works = std::move(state->queued_work_items);
        lock.unlock();

        ExecuteQueuedWorkItems(executed_works);
    }

    // All queued works should be executed before the thread exits.
    // The lock is already held here, so no need to lock again.
    ProcessDueDelayedWorkItems(state);
    auto executed_works = std::move(state->queued_work_items);
    lock.unlock();

    ExecuteQueuedWorkItems(executed_works);
}


std::optional<std::chrono::steady_clock::duration> 
    DefaultRunLoopThread::ProcessDueDelayedWorkItems(const std::shared_ptr<State>& state) {

    if (state->delayed_work_items.empty()) {
        return std::nullopt;
    }

    // Move due delayed works to the queued works before executing works, to avoid delayed work 
    // starvation.
    auto now = std::chrono::steady_clock::now();
    while (!state->delayed_work_items.empty()) {

        auto& first_item = state->delayed_work_items.front();
        if (first_item->ExecuteTimePoint() <= now) {

            state->queued_work_items.push_back(std::move(first_item));
            state->delayed_work_items.pop_front();
        }
        else {
            return first_item->ExecuteTimePoint() - now;
        }
    }

    return std::nullopt;
}


void DefaultRunLoopThread::ExecuteQueuedWorkItems(
    const std::vector<std::shared_ptr<WorkItem>>& work_items) {

    for (const auto& each_item : work_items) {
        auto work = each_item->TakeWorkIfNotDisposed();
        if (work) {
            work();
        }
    }
}


DefaultRunLoopThread::DelayedWorkItem::DelayedWorkItem(
    std::chrono::steady_clock::time_point execute_time_point,
    Closure work,
    std::weak_ptr<State> state) noexcept
    :
    WorkItem(std::move(work)),
    execute_time_point_(execute_time_point),
    state_(std::move(state)) {

}


void DefaultRunLoopThread::DelayedWorkItem::OnDispose() noexcept {

    auto state = state_.lock();
    if (!state) {
        return;
    }

    bool need_notify{};
    {
        std::lock_guard<std::mutex> lock(state->lock);

        auto iterator = std::lower_bound(
            state->delayed_work_items.begin(),
            state->delayed_work_items.end(),
            this,
            [](const std::shared_ptr<DelayedWorkItem>& existing,
               DelayedWorkItem* current_item) {
                return existing->ExecuteTimePoint() < current_item->ExecuteTimePoint();
            });

        for (; iterator != state->delayed_work_items.end(); ++iterator) {

            if ((*iterator)->ExecuteTimePoint() != execute_time_point_) {
                break;
            }

            if ((*iterator).get() == this) {
                if (iterator == state->delayed_work_items.begin()) {
                    need_notify = true;
                }
                state->delayed_work_items.erase(iterator);
                break;
            }
        }
    }

    if (need_notify) {
        state->work_event.notify_one();
    }
}

}