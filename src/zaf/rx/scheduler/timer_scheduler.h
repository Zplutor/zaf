#pragma once

#include <zaf/rx/scheduler/single_thread_scheduler.h>

namespace zaf::rx::internal {
class RxRuntime;
}

namespace zaf::rx {

class TimerScheduler : public SingleThreadScheduler {
public:
    static const std::shared_ptr<TimerScheduler>& Instance() noexcept;

private:
    friend class internal::RxRuntime;

    explicit TimerScheduler(std::shared_ptr<RunLoopThread> timer_thread);
};

}