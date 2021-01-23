#pragma once

#include <zaf/rx/work.h>

namespace zaf {

class Scheduler {
public:
    static std::shared_ptr<Scheduler> MainThread();
    static std::shared_ptr<Scheduler> CreateOnSingleThread();

public:
    Scheduler() = default;
    virtual ~Scheduler() = default;

    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;

    virtual void Schedule(Work work) = 0;
};

}