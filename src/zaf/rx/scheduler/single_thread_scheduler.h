#pragma once

#include <zaf/rx/scheduler/scheduler.h>
#include <zaf/rx/thread/run_loop_thread.h>

namespace zaf::rx {

class SingleThreadScheduler : public Scheduler {
public:
    SingleThreadScheduler();
    explicit SingleThreadScheduler(std::shared_ptr<RunLoopThread> thread);

    void ScheduleWork(Closure work) override;
    void ScheduleDelayedWork(std::chrono::steady_clock::duration delay, Closure work) override;

private:
    std::shared_ptr<RunLoopThread> thread_;
};

}