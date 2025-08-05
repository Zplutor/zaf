#pragma once

#include <deque>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>
#include <zaf/rx/run_loop_thread.h>

namespace zaf::testing {
class DefaultRunLoopThreadTest;
}

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
    class DelayedWorkItem {
    public:
        std::chrono::steady_clock::time_point execute_time_point;
        Closure work;
    };

    class State {
    public:
        std::mutex lock;
        std::atomic<bool> is_stopped{};
        std::condition_variable work_event;

        // Works that need to be executed immediately.
        std::vector<Closure> queued_works;

        // Works that need to be executed after a delay.
        // Sorted by execute time point.
        std::deque<DelayedWorkItem> delayed_works;
    };

private:
    static void ThreadProcedure(const std::shared_ptr<State>& state);
    static std::optional<std::chrono::steady_clock::duration> ProcessDueDelayedWorks(
        const std::shared_ptr<State>& state);
    static void ExecuteQueuedWorks(std::vector<Closure>& queued_works);

private:
    friend class zaf::testing::DefaultRunLoopThreadTest;

    void PostWorkAt(std::chrono::steady_clock::time_point execute_time_point, Closure work);

private:
    std::shared_ptr<State> state_;
    std::thread thread_;
};

}