#include <zaf/rx/internal/timer_manager.h>
#include <zaf/base/container/utility/erase.h>
#include <zaf/base/container/utility/sort.h>

namespace zaf::rx::internal {

TimerManager::TimerManager() : 
    thread_state_(std::make_shared<ThreadState>()),
    timer_thread_(std::bind(ThreadEntry, thread_state_)) {

}


TimerManager::~TimerManager() {

    thread_state_->is_stopped.store(true);
    thread_state_->event.notify_one();

    timer_thread_.join();
}


TimerManager::TimerId TimerManager::SetTimer(
    std::chrono::steady_clock::time_point time_point,
    std::shared_ptr<Scheduler> scheduler,
    TimerWork timer_work) {

    auto timer_id = ++timer_id_seed_;

    Timer timer;
    timer.id = timer_id;
    timer.time_point = std::move(time_point);
    timer.scheduler = std::move(scheduler);
    timer.work = std::move(timer_work);

    std::scoped_lock<std::mutex> lock(thread_state_->lock);

    bool need_notify{};
    if (thread_state_->timers.empty()) {
        need_notify = true;
    }
    else {
        if (timer.time_point < thread_state_->timers.front().time_point) {
            need_notify = true;
        }
    }

    thread_state_->timers.push_back(std::move(timer));

    Sort(thread_state_->timers, [](const Timer& timer1, const Timer& timer2) {
        return timer1.time_point < timer2.time_point;
    });

    if (need_notify) {
        thread_state_->event.notify_one();
    }

    return timer_id;
}


void TimerManager::CancelTimer(TimerId id) {

    std::scoped_lock<std::mutex> lock(thread_state_->lock);
    if (thread_state_->timers.empty()) {
        return;
    }

    bool need_notify{};
    if (thread_state_->timers.front().id == id) {
        need_notify = true;
    }

    EraseIf(thread_state_->timers, [id](const Timer& timer) {
        return timer.id == id;
    });

    if (need_notify) {
        thread_state_->event.notify_one();
    }
}


void TimerManager::ThreadEntry(const std::shared_ptr<ThreadState>& state) {

    std::unique_lock<std::mutex> lock(state->lock);
    while (!state->is_stopped.load()) {

        if (state->timers.empty()) {
            state->event.wait(lock);
            continue;
        }

        auto now = std::chrono::steady_clock::now();
        auto& first_timer = state->timers.front();
        if (now < first_timer.time_point) {
            state->event.wait_until(lock, first_timer.time_point);
            continue;
        }

        auto id = first_timer.id;
        auto scheduler = std::move(first_timer.scheduler);
        auto work = std::move(first_timer.work);
        state->timers.pop_front();

        lock.unlock();
        scheduler->ScheduleWork(std::bind(work, id));
        lock.lock();
    }
}

}