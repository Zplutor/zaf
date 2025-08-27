#include <zaf/rx/scheduler/immediate_scheduler.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/rx/disposable.h>

namespace zaf::rx {

const std::shared_ptr<ImmediateScheduler>& ImmediateScheduler::Instance() {
    static std::shared_ptr<ImmediateScheduler> instance{ new ImmediateScheduler{} };
    return instance;
}


std::shared_ptr<Disposable> ImmediateScheduler::ScheduleWork(Closure work) {
    ZAF_EXPECT(work);
    work();
    return Disposable::Empty();
}


std::shared_ptr<Disposable> ImmediateScheduler::ScheduleDelayedWork(
    std::chrono::steady_clock::duration delay, 
    Closure work) {

    ZAF_EXPECT(work);
    std::this_thread::sleep_for(delay);
    work();
    return Disposable::Empty();
}

}
