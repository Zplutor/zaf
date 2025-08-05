#include <zaf/rx/scheduler/single_thread_scheduler.h>
#include <zaf/rx/thread/default_run_loop_thread.h>

namespace zaf::rx {

SingleThreadScheduler::SingleThreadScheduler() : 
    thread_(std::make_shared<DefaultRunLoopThread>()) {

}


SingleThreadScheduler::SingleThreadScheduler(std::shared_ptr<RunLoopThread> thread) :
    thread_(std::move(thread)) {

}


void SingleThreadScheduler::ScheduleWork(Closure work) {
    thread_->PostWork(std::move(work));
}


void SingleThreadScheduler::ScheduleDelayedWork(
    std::chrono::steady_clock::duration delay, 
    Closure work) {

    thread_->PostDelayedWork(delay, std::move(work));
}

}