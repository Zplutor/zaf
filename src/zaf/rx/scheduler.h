#pragma once

#include <zaf/base/closure.h>
#include <zaf/base/non_copyable.h>

namespace zaf::rx {

class Scheduler : NonCopyableNonMovable {
public:
    static std::shared_ptr<Scheduler> Main();
    static std::shared_ptr<Scheduler> Timer();
    static std::shared_ptr<Scheduler> CreateOnSingleThread();

public:
    Scheduler() = default;
    virtual ~Scheduler() = default;

    virtual void Schedule(Closure work) = 0;
};

}