#include <zaf/rx/scheduler/trampoline_scheduler.h>
#include <deque>
#include <optional>
#include <zaf/base/auto_reset.h>
#include <zaf/base/error/precondition_error.h>
#include <zaf/rx/internal/thread/thread_work_item_base.h>

namespace zaf::rx {
namespace {

class TrampolineSchedulerCore {
public:
    class QueuedWorkItem : public internal::ThreadWorkItemBase {
    public:
        QueuedWorkItem(
            Closure work, 
            std::optional<std::chrono::steady_clock::time_point> execute_time_point) noexcept 
            :
            ThreadWorkItemBase(std::move(work)),
            execute_time_point_(execute_time_point) {
        }

        const std::optional<std::chrono::steady_clock::time_point>& 
            ExecuteTimePoint() const noexcept {
            return execute_time_point_;
        }

    protected:
        void OnDispose() noexcept override {
            // Nothing to do.
        }

    private:
        std::optional<std::chrono::steady_clock::time_point> execute_time_point_;
    };

public:
    std::shared_ptr<Disposable> ScheduleWorkItem(
        Closure work,
        std::optional<std::chrono::steady_clock::time_point> execute_time_point) {

        ZAF_EXPECT(work);

        if (is_executing_work_) {
            auto work_item = std::make_shared<QueuedWorkItem>(std::move(work), execute_time_point);
            work_queue_.push_back(work_item);
            return work_item;
        }

        auto auto_reset = zaf::MakeAutoReset(is_executing_work_, true);
        RunImmediateWork(work, execute_time_point);
        DrainWorkQueue();
        return Disposable::Empty();
    }

    void RunImmediateWork(
        const Closure& work,
        std::optional<std::chrono::steady_clock::time_point> execute_time_point) {

        SleepUntil(execute_time_point);
        work();
    }

    void DrainWorkQueue() {

        while (!work_queue_.empty()) {

            auto first_item = std::move(work_queue_.front());
            work_queue_.pop_front();

            RunQueuedWork(*first_item);
        }
    }

    void RunQueuedWork(QueuedWorkItem& work) {

        if (work.IsDisposed()) {
            return;
        }

        SleepUntil(work.ExecuteTimePoint());
        work.RunWork();
    }

    void SleepUntil(std::optional<std::chrono::steady_clock::time_point> execute_time_point) {

        if (!execute_time_point) {
            return;
        }

        auto now = std::chrono::steady_clock::now();
        if (now >= *execute_time_point) {
            return;
        }

        auto wait_duration = *execute_time_point - now;
        std::this_thread::sleep_for(wait_duration);
    }

private:
    std::deque<std::shared_ptr<QueuedWorkItem>> work_queue_;
    bool is_executing_work_{};
};

thread_local TrampolineSchedulerCore trampoline_scheduler_core;

} // namespace


const std::shared_ptr<TrampolineScheduler>& TrampolineScheduler::Instance() {
    static std::shared_ptr<TrampolineScheduler> instance{ new TrampolineScheduler };
    return instance;
}


std::shared_ptr<Disposable> TrampolineScheduler::ScheduleWork(Closure work) {
    return trampoline_scheduler_core.ScheduleWorkItem(std::move(work), std::nullopt);
}


std::shared_ptr<Disposable> TrampolineScheduler::ScheduleDelayedWork(
    std::chrono::steady_clock::duration delay, 
    Closure work) {

    return trampoline_scheduler_core.ScheduleWorkItem(
        std::move(work),
        std::chrono::steady_clock::now() + delay);
}

}