#include <zaf/rx/scheduler/immediate_scheduler.h>

namespace zaf::rx {

const std::shared_ptr<ImmediateScheduler>& ImmediateScheduler::Instance() {
    static std::shared_ptr<ImmediateScheduler> instance{ new ImmediateScheduler{} };
    return instance;
}


void ImmediateScheduler::ScheduleWork(Closure work) {
    work();
}


std::shared_ptr<Disposable> ImmediateScheduler::ScheduleDelayedWork(
    std::chrono::steady_clock::duration delay, 
    Closure work) {

    std::this_thread::sleep_for(delay);
    work();
    return nullptr;
}

}
