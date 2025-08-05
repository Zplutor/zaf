#include "default_run_loop_thread.h"
#include "default_run_loop_thread.h"
#include <zaf/rx/default_run_loop_thread.h>

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


void DefaultRunLoopThread::PostWork(Closure work) {

    {
        std::lock_guard<std::mutex> lock(state_->lock);
        state_->queued_works.push_back(std::move(work));
    }
    state_->work_event.notify_one();
}


void DefaultRunLoopThread::PostDelayedWork(
    std::chrono::steady_clock::duration delay, 
    Closure work) {

    PostWorkAt(std::chrono::steady_clock::now() + delay, std::move(work));
}


void DefaultRunLoopThread::PostWorkAt(
    std::chrono::steady_clock::time_point execute_time_point, 
    Closure work) {

    DelayedWorkItem work_item;
    work_item.execute_time_point = execute_time_point;
    work_item.work = std::move(work);

    {
        std::lock_guard<std::mutex> lock(state_->lock);

        auto iterator = std::lower_bound(
            state_->delayed_works.begin(),
            state_->delayed_works.end(),
            work_item.execute_time_point,
            [](const DelayedWorkItem& item,
                const std::chrono::steady_clock::time_point& time_point) {
                return item.execute_time_point < time_point;
            });

        // The new work item should be inserted after the item which has the same execute time 
        // point. 
        if ((iterator != state_->delayed_works.end()) &&
            (iterator->execute_time_point == work_item.execute_time_point)) {

            state_->delayed_works.insert(std::next(iterator), std::move(work_item));
        }
        else {
            state_->delayed_works.insert(iterator, std::move(work_item));
        }
    }

    state_->work_event.notify_one();
}


void DefaultRunLoopThread::ThreadProcedure(const std::shared_ptr<State>& state) {

    std::unique_lock<std::mutex> lock(state->lock, std::defer_lock);
    while (true) {

        lock.lock();
        if (state->is_stopped) {
            break;
        }

        auto wait_duration = ProcessDueDelayedWorks(state);

        if (state->queued_works.empty()) {
            if (wait_duration) {
                state->work_event.wait_for(lock, *wait_duration);
            }
            else {
                state->work_event.wait(lock);
            }
        }

        auto executed_works = std::move(state->queued_works);
        lock.unlock();

        ExecuteQueuedWorks(executed_works);
    }

    // All queued works should be executed before the thread exits.
    // The lock is already held here, so no need to lock again.
    ProcessDueDelayedWorks(state);
    auto executed_works = std::move(state->queued_works);
    lock.unlock();

    ExecuteQueuedWorks(executed_works);
}


std::optional<std::chrono::steady_clock::duration> DefaultRunLoopThread::ProcessDueDelayedWorks(
    const std::shared_ptr<State>& state) {

    if (state->delayed_works.empty()) {
        return std::nullopt;
    }

    // Move due delayed works to the queued works before executing works, to avoid delayed work 
    // starvation.
    auto now = std::chrono::steady_clock::now();
    while (!state->delayed_works.empty()) {

        auto& first_item = state->delayed_works.front();
        if (first_item.execute_time_point <= now) {

            state->queued_works.push_back(std::move(first_item.work));
            state->delayed_works.pop_front();
        }
        else {
            return first_item.execute_time_point - now;
        }
    }

    return std::nullopt;
}


void DefaultRunLoopThread::ExecuteQueuedWorks(std::vector<Closure>& queued_works) {
    for (auto& each_work : queued_works) {
        each_work();
        // Free the memory of the work immediately after execution.
        each_work = nullptr;
    }
}

}