#pragma once

#include <deque>
#include <mutex>
#include <thread>
#include <zaf/rx/run_loop_thread.h>

namespace zaf::rx {

/**
Represents a run loop thread with a default implementation.
*/
class DefaultRunLoopThread : public RunLoopThread {
public:
    DefaultRunLoopThread();
    ~DefaultRunLoopThread();

    void PostWork(Closure work) override;
    void PostDelayedWork(std::chrono::steady_clock::duration delay, Closure work) override;

private:
    class State {
    public:
        std::atomic<bool> is_stopped{};
        std::deque<Closure> works;
        std::mutex works_lock;
        std::condition_variable work_event;
    };

private:
    static void Run(const std::shared_ptr<State>& state);

private:
    std::shared_ptr<State> state_;
    std::thread thread_;
};

}