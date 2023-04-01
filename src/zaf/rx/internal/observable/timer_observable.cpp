#include <zaf/rx/internal/observable/timer_observable.h>
#include <zaf/rx/internal/subscription/subscription_core.h>
#include <zaf/rx/internal/rx_runtime.h>
#include <zaf/rx/internal/timer_manager.h>

namespace zaf::internal {
namespace {

class TimerSubscriptionCore : 
    public SubscriptionCore, 
    public std::enable_shared_from_this<TimerSubscriptionCore> {

public:
    TimerSubscriptionCore(
        std::chrono::steady_clock::duration delay,
        std::optional<std::chrono::steady_clock::duration> interval,
        std::shared_ptr<Scheduler> scheduler,
        std::shared_ptr<InnerObserver> observer)
        :
        SubscriptionCore(std::move(observer)),
        delay_(std::move(delay)),
        interval_(std::move(interval)),
        scheduler_(std::move(scheduler)) {

    }

    void Subscribe() {

        begin_time_point_ = std::chrono::steady_clock::now();
        SetTimer();
    }

    void OnUnsubscribe() override {

        is_unsubscribed_.store(true);
        RxRuntime::GetInstance().GetTimerManager().CancelTimer(timer_id_);
    }

private:
    void SetTimer() {

        auto& timer_manager = RxRuntime::GetInstance().GetTimerManager();
        timer_id_ = timer_manager.SetTimer(
            GetNextTimePoint(),
            scheduler_,
            std::bind(&TimerSubscriptionCore::OnTimer, shared_from_this()));
    }

    std::chrono::steady_clock::time_point GetNextTimePoint() {

        auto delay_end = begin_time_point_ + delay_;
        if (!interval_) {
            return delay_end;
        }

        auto interval_end = delay_end + (*interval_ * trigger_count_);

        auto now = std::chrono::steady_clock::now();
        if (now <= interval_end) {
            return interval_end;
        }

        auto gap = now - interval_end;
        auto interval_count = gap / *interval_;
        if (interval_count == 0) {
            return interval_end;
        }

        auto adjust_interval = (*interval_ * interval_count);
        auto new_interval_end = delay_end + adjust_interval;

        begin_time_point_ += adjust_interval;
        return new_interval_end;
    }

    void OnTimer() {

        if (is_unsubscribed_.load()) {
            return;
        }

        DeliverOnNext(trigger_count_);
        trigger_count_++;

        if (!interval_) {
            DeliverOnCompleted();
            return;
        }

        SetTimer();
    }

private:
    std::chrono::steady_clock::duration delay_;
    std::optional<std::chrono::steady_clock::duration> interval_;
    std::shared_ptr<Scheduler> scheduler_;
    std::chrono::steady_clock::time_point begin_time_point_;
    int trigger_count_{};
    TimerManager::TimerId timer_id_{};
    std::atomic<bool> is_unsubscribed_{};
};

}

TimerObservable::TimerObservable(
    std::chrono::steady_clock::duration delay,
    std::optional<std::chrono::steady_clock::duration> interval,
    std::shared_ptr<Scheduler> scheduler) 
    :
    delay_(std::move(delay)),
    interval_(std::move(interval)),
    scheduler_(std::move(scheduler)) {

}


std::shared_ptr<SubscriptionCore> TimerObservable::Subscribe(
    const std::shared_ptr<InnerObserver>& observer) {

    auto subscription_core = std::make_shared<TimerSubscriptionCore>(
        delay_,
        interval_, 
        scheduler_, 
        observer);

    subscription_core->Subscribe();
    return subscription_core;
}

}