#pragma once

#include <chrono>
#include <zaf/base/closure.h>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/disposable.h>

namespace zaf::rx {

class Scheduler : NonCopyableNonMovable {
public:
    Scheduler() = default;
    virtual ~Scheduler() = default;

    virtual std::shared_ptr<Disposable> ScheduleWork(Closure work) = 0;

    virtual std::shared_ptr<Disposable> ScheduleDelayedWork(
        std::chrono::steady_clock::duration delay, 
        Closure work) = 0;
};

}