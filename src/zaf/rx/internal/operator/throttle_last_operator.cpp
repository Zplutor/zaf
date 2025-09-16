#include <zaf/rx/internal/operator/throttle_last_operator.h>
#include <zaf/base/as.h>
#include <zaf/base/error/precondition_error.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/producer.h>
#include <zaf/rx/scheduler/scheduler.h>

namespace zaf::rx::internal {
namespace {

class ThrottleLastProducer : public Producer, public ObserverCore {
public:
    ThrottleLastProducer(
        ObserverShim&& observer,
        std::chrono::steady_clock::duration duration,
        std::shared_ptr<Scheduler> scheduler)
        :
        Producer(std::move(observer)),
        duration_(duration),
        scheduler_(std::move(scheduler)),
        timer_(Disposable::Empty()) {

    }

    ~ThrottleLastProducer() {
        DoDisposal();
    }

    void Run(const std::shared_ptr<ObservableCore>& source) {
        source_sub_ = source->Subscribe(
            ObserverShim::FromShared(As<ObserverCore>(shared_from_this())));
    }

    void OnNext(const std::any& value) override {
        try {

            {
                std::lock_guard<std::mutex> lock(mutex_);
                last_value_ = value;
            }

            auto empty_timer = Disposable::Empty();
            if (timer_.load() != empty_timer) {
                // There is already a timer running, or the producer is disposed.
                return;
            }

            std::weak_ptr<ThrottleLastProducer> weak_this =
                As<ThrottleLastProducer>(shared_from_this());

            auto new_timer = scheduler_->ScheduleDelayedWork(duration_, [weak_this]() {
                if (auto strong_this = weak_this.lock()) {
                    strong_this->OnTimer();
                }
            });

            if (!timer_.compare_exchange_strong(empty_timer, new_timer)) {
                // Another thread has set a new timer or disposed the producer.
                new_timer->Dispose();
            }
        }
        catch (...) {
            EmitOnError(std::current_exception());
        }
    }

    void OnError(const std::exception_ptr& error) override {
        CancelTimer();
        EmitOnError(error);
    }

    void OnCompleted() override {
        CancelTimer();
        EmitLastValue();
        EmitOnCompleted();
    }

protected:
    void OnDispose() noexcept override {
        DoDisposal();
    }

private:
    void OnTimer() {

        auto old_timer = timer_.exchange(Disposable::Empty());
        if (!old_timer) {
            return;
        }
        old_timer->Dispose();

        EmitLastValue();
    }

    void EmitLastValue() {

        std::any value;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            value = std::move(last_value_);
        }

        if (value.has_value()) {
            EmitOnNext(value);
        }
    }

    void CancelTimer() noexcept {
        auto old_timer = timer_.exchange(nullptr);
        if (old_timer) {
            old_timer->Dispose();
        }
    }

    void DoDisposal() noexcept {
        CancelTimer();
        if (source_sub_) {
            source_sub_->Dispose();
            source_sub_.reset();
        }
        scheduler_.reset();
    }

private:
    std::chrono::steady_clock::duration duration_{};
    std::shared_ptr<Scheduler> scheduler_{};

    std::shared_ptr<Disposable> source_sub_;
    std::atomic<std::shared_ptr<Disposable>> timer_;

    std::mutex mutex_;
    std::any last_value_;
};

} // namespace

ThrottleLastOperator::ThrottleLastOperator(
    std::shared_ptr<ObservableCore> source, 
    std::chrono::steady_clock::duration duration,
    std::shared_ptr<Scheduler> scheduler)
    :
    source_(std::move(source)),
    duration_(duration),
    scheduler_(std::move(scheduler)) {

    ZAF_EXPECT(source_);
    ZAF_EXPECT(scheduler_);
}


std::shared_ptr<Disposable> ThrottleLastOperator::Subscribe(ObserverShim&& observer) {

    auto producer = std::make_shared<ThrottleLastProducer>(
        std::move(observer), 
        duration_, 
        scheduler_);

    producer->Run(source_);
    return producer;
}

}