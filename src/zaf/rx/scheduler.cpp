#include <zaf/rx/scheduler.h>
#include <zaf/application.h>
#include <zaf/rx/internal/scheduler/single_thread_scheduler.h>
#include <zaf/rx/internal/thread/thread_manager.h>

namespace zaf {

std::shared_ptr<Scheduler> Scheduler::MainThread() {

    static auto main_thread_scheduler = std::make_shared<internal::SingleThreadScheduler>(
        Application::Instance().GetThreadManager().GetMainThread());

    return main_thread_scheduler;
}


std::shared_ptr<Scheduler> Scheduler::CreateOnSingleThread() {

    auto thread = Application::Instance().GetThreadManager().CreateNewThread();
    return std::make_shared<internal::SingleThreadScheduler>(thread);
}

}