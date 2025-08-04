#pragma once

#include <chrono>
#include <zaf/base/closure.h>
#include <zaf/base/non_copyable.h>

namespace zaf::rx {

class RunLoopThread : NonCopyableNonMovable {
public:
    RunLoopThread() = default;
    virtual ~RunLoopThread() = default;

    virtual void PostWork(Closure work) = 0;
    virtual void PostDelayedWork(std::chrono::steady_clock::duration delay, Closure work) = 0;
};

}