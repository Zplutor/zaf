#pragma once

#include <zaf/rx/scheduler.h>
#include <zaf/rx/run_loop_thread.h>

namespace zaf::rx::internal {

class SingleThreadScheduler : public Scheduler {
public:
    explicit SingleThreadScheduler(const std::shared_ptr<RunLoopThread>& thread) : thread_(thread) {

    }

    void Schedule(Closure work) override {
        thread_->PostWork(std::move(work));
    }

private:
    std::shared_ptr<RunLoopThread> thread_;
};

}