#pragma once

#include <zaf/rx/scheduler/scheduler.h>
#include <zaf/rx/thread/run_loop_thread.h>

namespace zaf::rx {

class SingleThreadScheduler : public Scheduler {
public:
    SingleThreadScheduler();
    explicit SingleThreadScheduler(std::shared_ptr<RunLoopThread> thread);

    void Schedule(Closure work) override;

private:
    std::shared_ptr<RunLoopThread> thread_;
};

}