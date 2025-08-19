#pragma once

#include <zaf/rx/schedulers/scheduler.h>
#include <zaf/rx/threads/run_loop_thread.h>

namespace zaf::rx {

class SingleThreadScheduler : public Scheduler {
public:
    SingleThreadScheduler();
    explicit SingleThreadScheduler(std::shared_ptr<RunLoopThread> thread);

    void ScheduleWork(Closure work) override;

    std::shared_ptr<Disposable> ScheduleDelayedWork(
        std::chrono::steady_clock::duration delay, 
        Closure work) override;

private:
    std::shared_ptr<RunLoopThread> thread_;
};

}