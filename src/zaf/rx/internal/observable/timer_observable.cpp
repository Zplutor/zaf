#include <zaf/rx/internal/observable/timer_observable.h>
#include <zaf/base/as.h>
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
        scheduler_(std::move(scheduler)),
        atomic_timer_(Disposable::Empty()) {

    }

    ~TimerProducer() {
        DoDisposal();
    }

    void Run() {
        SetNextDelayTimer();
    }

    void OnDispose() noexcept override {
        DoDisposal();
    }

private:
    /* 
    This method will be called in the scheduler thread, which may cause data race with the
    DoDisposal method. We use the following strategies to resolve the issue:
    1. Use std::atomic<std::shared_ptr<Disposable>> to hold the timer disposable.
    2. Use Disposable::Empty() to indicate that the timer is not disposed yet and is ready to be 
       set.
    3. Use nullptr to indicate that the timer is disposed.
    4. Use compare_exchange_strong to set a new timer. If the current value is not 
       Disposable::Empty(), it means that the timer is disposed, so we should not set a new timer.
    */
    void SetNextDelayTimer() {

        auto next_delay = GetNextDelay();

        std::weak_ptr<TimerProducer> weak_this = As<TimerProducer>(shared_from_this());
        auto new_timer = scheduler_->ScheduleDelayedWork(next_delay, [weak_this]() {
            auto shared_this = weak_this.lock();
            if (shared_this) {
                shared_this->OnTimer();
            }
        });

        auto expected = Disposable::Empty();
        if (!atomic_timer_.compare_exchange_strong(expected, new_timer)) {
            new_timer->Dispose();
        }
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

        // Reset the timer to Disposable::Empty() to make it ready to be set again.
        auto previous_timer = atomic_timer_.exchange(Disposable::Empty());
        if (previous_timer == nullptr) {
            // The timer is disposed, do nothing.
            return;
        }

        auto on_next_value = emission_value_.fetch_add(1);
        EmitOnNext(on_next_value);

        if (!interval_) {
            EmitOnCompleted();
            return;
        }

        try {
            SetNextDelayTimer();
        }
        catch (...) {
            // If an error occurs while setting the next timer, emit the error.
            EmitOnError(std::current_exception());
        }
    }

    void DoDisposal() noexcept {

        auto timer = atomic_timer_.exchange(nullptr);
        if (timer) {
            timer->Dispose();
        }
    }

private:
    const std::chrono::steady_clock::duration delay_{};
    const std::optional<std::chrono::steady_clock::duration> interval_;
    const std::shared_ptr<Scheduler> scheduler_;
    std::atomic<std::shared_ptr<Disposable>> atomic_timer_;
    std::atomic<std::size_t> emission_value_{};
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


std::shared_ptr<Disposable> TimerObservable::Subscribe(ObserverShim&& observer) {

    auto producer = std::make_shared<TimerProducer>(
        delay_,
        interval_, 
        scheduler_, 
        std::move(observer));

    producer->Run();
    return producer;
}

}