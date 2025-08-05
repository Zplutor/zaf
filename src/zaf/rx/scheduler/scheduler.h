#pragma once

#include <zaf/base/closure.h>
#include <zaf/base/non_copyable.h>

namespace zaf::rx {

class Scheduler : NonCopyableNonMovable {
public:
    Scheduler() = default;
    virtual ~Scheduler() = default;

    virtual void Schedule(Closure work) = 0;
};

}