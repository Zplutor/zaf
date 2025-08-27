#include <zaf/rx/scheduler/single_thread_scheduler.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/rx/thread/default_run_loop_thread.h>

namespace zaf::rx {

SingleThreadScheduler::SingleThreadScheduler() : 
    thread_(std::make_shared<DefaultRunLoopThread>()) {

}


SingleThreadScheduler::SingleThreadScheduler(std::shared_ptr<RunLoopThread> thread) :
    thread_(std::move(thread)) {

    ZAF_EXPECT(thread_);
}


std::shared_ptr<Disposable> SingleThreadScheduler::ScheduleWork(Closure work) {
    return thread_->PostWork(std::move(work));
}


std::shared_ptr<Disposable> SingleThreadScheduler::ScheduleDelayedWork(
    std::chrono::steady_clock::duration delay, 
    Closure work) {

    return thread_->PostDelayedWork(delay, std::move(work));
}

}