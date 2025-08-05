#include <zaf/rx/scheduler/timer_scheduler.h>
#include <zaf/rx/internal/rx_runtime.h>

namespace zaf::rx {

const std::shared_ptr<TimerScheduler>& TimerScheduler::Instance() noexcept {
    return internal::RxRuntime::GetInstance().TimerScheduler();
}


TimerScheduler::TimerScheduler(std::shared_ptr<RunLoopThread> timer_thread) :
    SingleThreadScheduler(std::move(timer_thread)) {

}

}