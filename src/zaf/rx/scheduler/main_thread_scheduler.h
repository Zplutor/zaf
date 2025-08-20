#pragma once

#include <zaf/rx/scheduler/single_thread_scheduler.h>
#include <zaf/rx/thread/main_thread.h>

namespace zaf::rx::internal {
class RxRuntime;
}

namespace zaf::rx {

class MainThreadScheduler : public SingleThreadScheduler {
public:
    static const std::shared_ptr<MainThreadScheduler>& Instance() noexcept;

private:
    friend class internal::RxRuntime;

    explicit MainThreadScheduler(std::shared_ptr<MainThread> main_thread);
};

}