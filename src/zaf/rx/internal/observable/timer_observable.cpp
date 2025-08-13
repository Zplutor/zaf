#include <zaf/rx/internal/observable/timer_observable.h>
#include <zaf/base/as.h>
#include <zaf/rx/internal/subscription/producer_subscription_core.h>
#include <zaf/rx/internal/producer.h>
#include <zaf/rx/scheduler/scheduler.h>

namespace zaf::rx::internal {
namespace {

class TimerProducer : public Producer {
public:
    TimerProducer(
        std::chrono::steady_clock::duration delay,
        std::optional<std::chrono::steady_clock::duration> interval,
        std::shared_ptr<Scheduler> scheduler,
        ObserverShim&& observer)
        :
        Producer(std::move(observer)),
        delay_(std::move(delay)),
        interval_(std::move(interval)),
        scheduler_(std::move(scheduler)) {

    }

    void Run() {
        SetNextDelayTimer();
    }

    void OnDispose() noexcept override {
        is_unsubscribed_ = true;
        if (timer_) {
            timer_->Dispose();
            timer_.reset();
        }
    }

private:
    void SetNextDelayTimer() {

        auto next_delay = GetNextDelay();
        timer_ = scheduler_->ScheduleDelayedWork(
            next_delay,
            std::bind(&TimerProducer::OnTimer, As<TimerProducer>(shared_from_this())));
    }

    std::chrono::steady_clock::duration GetNextDelay() {
        if (emission_value_ == 0) {
            return delay_;
        }
        else if (interval_) {
            return *interval_;
        }
        // Should not reach here.
        return std::chrono::steady_clock::duration::zero();
    }

    void OnTimer() {

        if (is_unsubscribed_) {
            return;
        }

        auto on_next_value = emission_value_.fetch_add(1);
        EmitOnNext(on_next_value);

        if (!interval_) {
            EmitOnCompleted();
            return;
        }

        SetNextDelayTimer();
    }

private:
    std::chrono::steady_clock::duration delay_{};
    std::optional<std::chrono::steady_clock::duration> interval_;
    std::shared_ptr<Scheduler> scheduler_;
    std::shared_ptr<Disposable> timer_;
    std::atomic<std::size_t> emission_value_{};
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


std::shared_ptr<SubscriptionCore> TimerObservable::Subscribe(ObserverShim&& observer) {

    auto producer = std::make_shared<TimerProducer>(
        delay_,
        interval_, 
        scheduler_, 
        std::move(observer));

    producer->Run();
    return std::make_shared<ProducerSubscriptionCore>(std::move(producer));
}

}