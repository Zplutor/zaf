#pragma once

#include <zaf/rx/schedulers/scheduler.h>

namespace zaf::rx {

class ImmediateScheduler : public Scheduler {
public:
    static const std::shared_ptr<ImmediateScheduler>& Instance();

public:
    void ScheduleWork(Closure work) override;

    std::shared_ptr<Disposable> ScheduleDelayedWork(
        std::chrono::steady_clock::duration delay,
        Closure work) override;

private:
    ImmediateScheduler() noexcept = default;
};

}
