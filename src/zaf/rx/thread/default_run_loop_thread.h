#pragma once

#include <deque>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>
#include <zaf/rx/internal/thread/delayed_work_item_base.h>
#include <zaf/rx/thread/run_loop_thread.h>

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

    std::shared_ptr<Disposable> PostDelayedWork(
        std::chrono::steady_clock::duration delay, 
        Closure work) override;

private:
    class DelayedWorkItem;

    class State {
    public:
        std::mutex lock;
        std::atomic<bool> is_stopped{};
        std::condition_variable work_event;

        // Works that need to be executed immediately.
        std::vector<Closure> queued_works;

        // Works that need to be executed after a delay.
        // Sorted by execute time point.
        std::deque<std::shared_ptr<DelayedWorkItem>> delayed_works;
    };

    class DelayedWorkItem : public internal::DelayedWorkItemBase {
    public:
        DelayedWorkItem(
            std::chrono::steady_clock::time_point execute_time_point,
            Closure work,
            std::weak_ptr<State> state) noexcept;

        const std::chrono::steady_clock::time_point& ExecuteTimePoint() const noexcept {
            return execute_time_point_;
        }

    protected:
        void OnDispose() noexcept override;

    private:
        std::chrono::steady_clock::time_point execute_time_point_;
        std::weak_ptr<State> state_;
    };

private:
    static void ThreadProcedure(const std::shared_ptr<State>& state);
    static std::optional<std::chrono::steady_clock::duration> ProcessDueDelayedWorks(
        const std::shared_ptr<State>& state);
    static void ExecuteQueuedWorks(std::vector<Closure>& queued_works);

private:
    friend class zaf::testing::DefaultRunLoopThreadTest;

    std::shared_ptr<Disposable> PostWorkAt(
        std::chrono::steady_clock::time_point execute_time_point, 
        Closure work);

private:
    std::shared_ptr<State> state_;
    std::thread thread_;
};

}