#include <zaf/rx/internal/observable/timer_observable.h>
#include <zaf/base/as.h>
#include <zaf/rx/internal/subscription/producer_subscription_core.h>
#include <zaf/rx/internal/producer.h>
#include <zaf/rx/internal/rx_runtime.h>
#include <zaf/rx/internal/timer_manager.h>

namespace zaf::rx::internal {
namespace {

class TimerProducer : public Producer {
public:
    TimerProducer(
        std::chrono::steady_clock::duration delay,
        std::optional<std::chrono::steady_clock::duration> interval,
        std::shared_ptr<Scheduler> scheduler,
        std::shared_ptr<ObserverCore> observer)
        :
        Producer(std::move(observer)),
        delay_(std::move(delay)),
        interval_(std::move(interval)),
        scheduler_(std::move(scheduler)) {

    }

    void Run() {
        SetTimer();
    }

    void OnUnsubscribe() noexcept override {

        is_unsubscribed_.store(true);
        RxRuntime::GetInstance().GetTimerManager().CancelTimer(timer_id_);
    }

private:
    void SetTimer() {

        auto& timer_manager = RxRuntime::GetInstance().GetTimerManager();
        timer_id_ = timer_manager.SetTimer(
            GetNextTimePoint(),
            scheduler_,
            std::bind(&TimerProducer::OnTimer, As<TimerProducer>(shared_from_this())));
    }

    std::chrono::steady_clock::time_point GetNextTimePoint() {
        auto now = std::chrono::steady_clock::now();
        if (emission_value_ == 0) {
            return now + delay_;
        }
        else if (interval_) {
            return now + *interval_;
        }
        // Should not reach here.
        return now;
    }

    void OnTimer() {

        if (is_unsubscribed_.load()) {
            return;
        }

        EmitOnNext(emission_value_);
        emission_value_++;

        if (!interval_) {
            EmitOnCompleted();
            return;
        }

        SetTimer();
    }

private:
    std::chrono::steady_clock::duration delay_;
    std::optional<std::chrono::steady_clock::duration> interval_;
    std::shared_ptr<Scheduler> scheduler_;
    std::size_t emission_value_{};
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
    const std::shared_ptr<ObserverCore>& observer) {

    auto producer = std::make_shared<TimerProducer>(
        delay_,
        interval_, 
        scheduler_, 
        observer);

    producer->Run();
    return std::make_shared<ProducerSubscriptionCore>(std::move(producer));
}

}