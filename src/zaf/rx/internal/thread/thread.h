#pragma once

#include <zaf/rx/work.h>

namespace zaf::rx::internal {

class Thread {
public:
    Thread() = default;
    virtual ~Thread() = default;

    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;

    virtual void DoWork(Work work) = 0;
};

}