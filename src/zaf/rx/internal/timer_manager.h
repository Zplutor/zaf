#pragma once

#include <cstdint>
#include <deque>
#include <thread>
#include <mutex>
#include <zaf/rx/scheduler.h>

namespace zaf::rx::internal {

class TimerManager {
public:
    using TimerId = std::uint64_t;
    using TimerWork = std::function<void(TimerId)>;

public:
    TimerManager();
    ~TimerManager();

    TimerManager(const TimerManager&) = delete;
    TimerManager& operator=(const TimerManager&) = delete;

    TimerId SetTimer(
        std::chrono::steady_clock::time_point time_point, 
        std::shared_ptr<Scheduler> scheduler, 
        TimerWork timer_work);

    void CancelTimer(TimerId id);

private:
    class Timer {
    public:
        TimerId id{};
        std::chrono::steady_clock::time_point time_point;
        std::shared_ptr<Scheduler> scheduler;
        TimerWork work;
    };

    class ThreadState {
    public:
        std::mutex lock;
        std::condition_variable event;
        std::deque<Timer> timers;
        std::atomic<bool> is_stopped{};
    };

private:
    static void ThreadEntry(const std::shared_ptr<ThreadState>& state);

private:
    std::shared_ptr<ThreadState> thread_state_;
    std::thread timer_thread_;
    std::atomic<TimerId> timer_id_seed_{};
};

}