#pragma once

#include <zaf/rx/scheduler.h>
#include <zaf/rx/internal/thread/thread.h>

namespace zaf::rx::internal {

class SingleThreadScheduler : public Scheduler {
public:
    explicit SingleThreadScheduler(const std::shared_ptr<Thread>& thread) : thread_(thread) {

    }

    void Schedule(Work work) override {
        thread_->DoWork(std::move(work));
    }

private:
    std::shared_ptr<Thread> thread_;
};

}