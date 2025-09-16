#include <zaf/rx/internal/operator/sample_operator.h>
#include <zaf/base/as.h>
#include <zaf/base/error/precondition_error.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/producer.h>
#include <zaf/rx/scheduler/scheduler.h>

namespace zaf::rx::internal {
namespace {

class SampleProducer : public Producer, public ObserverCore {
public:
    SampleProducer(
        ObserverShim&& observer,
        std::chrono::steady_clock::duration interval,
        std::shared_ptr<Scheduler> scheduler) noexcept
        :
        Producer(std::move(observer)),
        interval_(interval),
        scheduler_(std::move(scheduler)),
        timer_(Disposable::Empty()) {

    }

    ~SampleProducer() {
        DoDisposal();
    }

    void Run(const std::shared_ptr<ObservableCore> source) {

        source_sub_ = source->Subscribe(
            ObserverShim::FromShared(As<ObserverCore>(shared_from_this())));

        try {
            StartNextInterval();
        }
        catch (...) {
            source_sub_->Dispose();
            source_sub_.reset();
            throw;
        }
    }

    void OnNext(const std::any& value) override {
        std::lock_guard<std::mutex> lock(mutex_);
        last_value_ = value;
    }

    void OnError(const std::exception_ptr& error) override {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            last_value_.reset();
        }
        EmitOnError(error);
    }

    void OnCompleted() override {
        std::lock_guard<std::mutex> lock(mutex_);
        is_completed_ = true;
    }

private:
    void StartNextInterval() {

        std::weak_ptr<SampleProducer> weak_this = As<SampleProducer>(shared_from_this());
        auto new_timer = scheduler_->ScheduleDelayedWork(interval_, [weak_this]() {
            auto shared_this = weak_this.lock();
            if (shared_this) {
                shared_this->OnInterval();
            }
        });

        auto expected = Disposable::Empty();
        if (!timer_.compare_exchange_strong(expected, new_timer)) {
            new_timer->Dispose();
        }
    }

    void OnInterval() {

        auto previous_timer = timer_.exchange(Disposable::Empty());
        if (!previous_timer) {
            return;
        }

        if (!EmitLastValue()) {
            EmitOnCompleted();
            return;
        }

        try {
            StartNextInterval();
        }
        catch (...) {
            EmitOnError(std::current_exception());
        }
    }

    bool EmitLastValue() {
        std::any value;
        bool is_completed{};
        {
            std::lock_guard<std::mutex> lock(mutex_);
            value = std::move(last_value_);
            is_completed = is_completed_;
        }

        if (value.has_value()) {
            EmitOnNext(value);
        }
        return !is_completed;
    }

    void CancelTimer() {
        auto timer = timer_.exchange(nullptr);
        if (timer) {
            timer->Dispose();
        }
    }

    void OnDispose() noexcept {
        DoDisposal();
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
    std::chrono::steady_clock::duration interval_{};
    std::shared_ptr<Scheduler> scheduler_;

    std::shared_ptr<Disposable> source_sub_;
    std::atomic<std::shared_ptr<Disposable>> timer_;

    std::mutex mutex_;
    std::any last_value_;
    bool is_completed_{};
};

}

SampleOperator::SampleOperator(
    std::shared_ptr<ObservableCore> source,
    std::chrono::steady_clock::duration interval,
    std::shared_ptr<Scheduler> scheduler)
    :
    source_(std::move(source)),
    interval_(interval),
    scheduler_(std::move(scheduler)) {

    ZAF_EXPECT(source_);
    ZAF_EXPECT(scheduler_);
}


std::shared_ptr<Disposable> SampleOperator::Subscribe(ObserverShim&& observer) {
    auto producer = std::make_shared<SampleProducer>(std::move(observer), interval_, scheduler_);
    producer->Run(source_);
    return producer;
}

}