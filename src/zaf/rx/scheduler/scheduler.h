#pragma once

#include <chrono>
#include <zaf/base/closure.h>
#include <zaf/base/non_copyable.h>

namespace zaf::rx {

class Scheduler : NonCopyableNonMovable {
public:
    Scheduler() = default;
    virtual ~Scheduler() = default;

    virtual void ScheduleWork(Closure work) = 0;
    virtual void ScheduleDelayedWork(std::chrono::steady_clock::duration delay, Closure work) = 0;
};

}