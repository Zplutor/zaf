#include <zaf/rx/scheduler/trampoline_scheduler.h>
#include <deque>
#include <optional>
#include <zaf/base/auto_reset.h>

namespace zaf::rx {
namespace {

class TrampolineSchedulerCore {
public:
    class WorkItem : public Disposable {
    public:
        WorkItem(
            Closure work, 
            std::optional<std::chrono::steady_clock::time_point> execute_time_point) noexcept 
            :
            work_(std::move(work)),
            execute_time_point_(execute_time_point) {
        }

        bool IsDisposed() const noexcept override {
            return is_disposed.load();
        }

        Closure TakeWorkIfNotDisposed() noexcept {
            if (is_disposed.exchange(true)) {
                return nullptr;
            }
            return std::move(work_);
        }

        const std::optional<std::chrono::steady_clock::time_point>& 
            ExecuteTimePoint() const noexcept {
            return execute_time_point_;
        }

    protected:
        bool EnsureDisposed() noexcept override {
            
            if (is_disposed.exchange(true)) {
                return false;
            }

            work_ = nullptr;
            return true;
        }

    private:
        std::atomic<bool> is_disposed{};
        Closure work_;
        std::optional<std::chrono::steady_clock::time_point> execute_time_point_;
    };

public:
    std::shared_ptr<Disposable> ScheduleWorkItem(
        Closure work,
        std::optional<std::chrono::steady_clock::time_point> execute_time_point) {

        if (is_executing_work_) {
            auto work_item = std::make_shared<WorkItem>(std::move(work), execute_time_point);
            work_queue_.push_back(work_item);
            return work_item;
        }

        auto auto_reset = zaf::MakeAutoReset(is_executing_work_, true);
        ExecuteWorkItem(work, execute_time_point);
        DrainWorkQueue();
        return Disposable::Empty();
    }

    void DrainWorkQueue() {

        while (!work_queue_.empty()) {

            auto first_item = std::move(work_queue_.front());
            work_queue_.pop_front();

            auto work = first_item->TakeWorkIfNotDisposed();
            if (work) {
                ExecuteWorkItem(work, first_item->ExecuteTimePoint());
            }
        }
    }

    void ExecuteWorkItem(
        const Closure& work,
        const std::optional<std::chrono::steady_clock::time_point>& execute_time_point) {

        if (execute_time_point) {

            auto now = std::chrono::steady_clock::now();
            if (now < *execute_time_point) {

                auto wait_duration = *execute_time_point - now;
                std::this_thread::sleep_for(wait_duration);
            }
        }

        work();
    }

private:
    std::deque<std::shared_ptr<WorkItem>> work_queue_;
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