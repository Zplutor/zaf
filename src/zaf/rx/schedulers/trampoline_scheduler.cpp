#include <zaf/rx/schedulers/trampoline_scheduler.h>
#include <deque>
#include <optional>
#include <zaf/base/auto_reset.h>

namespace zaf::rx {
namespace {

class TrampolineSchedulerCore {
public:
    class WorkItem {
    public:
        Closure work;
        std::optional<std::chrono::steady_clock::time_point> execute_time_point;
    };

public:
    void ScheduleWorkItem(WorkItem work_item) {

        if (is_executing_work_) {
            work_queue_.push_back(std::move(work_item));
            return;
        }

        auto auto_reset = zaf::MakeAutoReset(is_executing_work_, true);
        ExecuteWorkItem(work_item);
        DrainWorkQueue();
    }

    void DrainWorkQueue() {

        while (!work_queue_.empty()) {

            auto first_item = std::move(work_queue_.front());
            work_queue_.pop_front();

            ExecuteWorkItem(first_item);
        }
    }

    void ExecuteWorkItem(const WorkItem& work_item) {

        if (work_item.execute_time_point) {
            auto now = std::chrono::steady_clock::now();
            if (now < *work_item.execute_time_point) {
                auto wait_duration = *work_item.execute_time_point - now;
                std::this_thread::sleep_for(wait_duration);
            }
        }
        work_item.work();
    }

private:
    std::deque<WorkItem> work_queue_;
    bool is_executing_work_{};
};

thread_local TrampolineSchedulerCore trampoline_scheduler_core;

} // namespace


const std::shared_ptr<TrampolineScheduler>& TrampolineScheduler::Instance() {
    static std::shared_ptr<TrampolineScheduler> instance{ new TrampolineScheduler };
    return instance;
}


void TrampolineScheduler::ScheduleWork(Closure work) {

    trampoline_scheduler_core.ScheduleWorkItem({
        .work = std::move(work),
    });
}


std::shared_ptr<Disposable> TrampolineScheduler::ScheduleDelayedWork(
    std::chrono::steady_clock::duration delay, 
    Closure work) {

    trampoline_scheduler_core.ScheduleWorkItem({
        .work = std::move(work),
        .execute_time_point = std::chrono::steady_clock::now() + delay,
    });
    return nullptr;
}

}