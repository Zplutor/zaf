#pragma once

#include <mutex>
#include <unordered_set>
#include <zaf/rx/internal/thread/thread_work_item_base.h>
#include <zaf/rx/scheduler/scheduler.h>
#include <zaf/rx/thread/run_loop_thread.h>

namespace zaf::testing {
class NewThreadSchedulerTest;
}

namespace zaf::rx {

class NewThreadScheduler : public Scheduler {
public:
    static const std::shared_ptr<NewThreadScheduler>& Default();

public:
    NewThreadScheduler();

    std::shared_ptr<Disposable> ScheduleWork(Closure work) override;

    std::shared_ptr<Disposable> ScheduleDelayedWork(
        std::chrono::steady_clock::duration delay,
        Closure work) override;

private:
    class WorkItem;

    class SharedState {
    public:
        std::mutex mutex;
        std::unordered_set<std::shared_ptr<WorkItem>> work_items;
    };

    class WorkItem :
        public internal::ThreadWorkItemBase,
        public std::enable_shared_from_this<WorkItem> {

    public:
        WorkItem(Closure work, std::weak_ptr<SharedState>);
        ~WorkItem();

        void RunOnThread(std::optional<std::chrono::steady_clock::duration> delay);

    protected:
        void OnDispose() noexcept override;

    private:
        std::weak_ptr<SharedState> shared_state_;
        std::unique_ptr<RunLoopThread> thread_;
        std::shared_ptr<Disposable> work_disposable_;
    };

private:
    std::shared_ptr<Disposable> ScheduleWorkOnThread(
        Closure work,
        std::optional<std::chrono::steady_clock::duration> delay);

    std::size_t ThreadCount() const noexcept;

private:
    std::shared_ptr<SharedState> shared_state_;

    friend class testing::NewThreadSchedulerTest;
};

}