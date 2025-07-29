#pragma once

#include <zaf/base/closure.h>

namespace zaf::rx {

class Scheduler {
public:
    static std::shared_ptr<Scheduler> Main();
    static std::shared_ptr<Scheduler> Timer();
    static std::shared_ptr<Scheduler> CreateOnSingleThread();

public:
    Scheduler() = default;
    virtual ~Scheduler() = default;

    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;

    virtual void Schedule(Closure work) = 0;
};

}