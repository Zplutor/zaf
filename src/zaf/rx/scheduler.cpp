#include <zaf/rx/scheduler.h>
#include <zaf/rx/internal/rx_runtime.h>
#include <zaf/rx/internal/scheduler/single_thread_scheduler.h>
#include <zaf/rx/internal/thread/thread_manager.h>

namespace zaf {

std::shared_ptr<Scheduler> Scheduler::Main() {

    static auto main_scheduler = std::make_shared<internal::SingleThreadScheduler>(
        internal::RxRuntime::GetInstance().GetThreadManager().GetMainThread());

    return main_scheduler;
}


std::shared_ptr<Scheduler> Scheduler::Timer() {

    static auto timer_scheduler = std::make_shared<internal::SingleThreadScheduler>(
        internal::RxRuntime::GetInstance().GetThreadManager().CreateNewThread());

    return timer_scheduler;
}


std::shared_ptr<Scheduler> Scheduler::CreateOnSingleThread() {

    auto thread = internal::RxRuntime::GetInstance().GetThreadManager().CreateNewThread();
    return std::make_shared<internal::SingleThreadScheduler>(thread);
}

}