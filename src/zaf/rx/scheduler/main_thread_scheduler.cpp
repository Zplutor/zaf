#include <zaf/rx/scheduler/main_thread_scheduler.h>
#include <zaf/rx/internal/rx_runtime.h>

namespace zaf::rx {

const std::shared_ptr<MainThreadScheduler>& MainThreadScheduler::Instance() noexcept {
    return internal::RxRuntime::GetInstance().MainThreadScheduler();
}


MainThreadScheduler::MainThreadScheduler(std::shared_ptr<MainThread> main_thread) :
    SingleThreadScheduler(std::move(main_thread)) {

}

}