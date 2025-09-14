#include <zaf/rx/internal/operator/debounce_operator.h>
#include <zaf/base/as.h>
#include <zaf/base/error/precondition_error.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::rx::internal {
namespace {

class DebounceProducer : public Producer, public ObserverCore {
public:
    DebounceProducer(
        ObserverShim&& observer, 
        std::chrono::steady_clock::duration duration,
        std::shared_ptr<Scheduler> scheduler)
        :
        Producer(std::move(observer)),
        duration_(duration),
        scheduler_(std::move(scheduler)),
        timer_(Disposable::Empty()) {

    }

    ~DebounceProducer() {
        DoDisposal();
    }

    void Run(const std::shared_ptr<ObservableCore>& source) {
        source_sub_ = source->Subscribe(
            ObserverShim::FromShared(As<ObserverCore>(shared_from_this())));
    }

    void OnNext(const std::any& value) override {

        std::size_t current_id{};
        std::shared_ptr<Disposable> last_timer;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            last_value_ = value;
            last_id_++;
            current_id = last_id_;
            last_timer = timer_.exchange(Disposable::Empty());
        }

        // If last timer is null, it means the producer is disposed, so we should abort.
        if (!last_timer) {
            return;
        }
        last_timer->Dispose();

        std::weak_ptr<DebounceProducer> weak_this = As<DebounceProducer>(shared_from_this());
        auto new_timer = scheduler_->ScheduleDelayedWork(duration_, [weak_this, current_id]() {
            if (auto shared_this = weak_this.lock()) {
                shared_this->OnDelayedWork(current_id);
            }
        });

        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (current_id != last_id_) {
                new_timer->Dispose();
                return;
            }

            last_timer = timer_.exchange(new_timer);
            if (!last_timer) {
                // Check if the producer is disposed again, dispose the new one and abort if so.
                new_timer->Dispose();
            }
        }
    }

    void OnError(const std::exception_ptr& error) override {
        CancelTimer();
        EmitOnError(error);
    }

    void OnCompleted() override {
        CancelTimer();
        EmitLastValueOnCompleted();
        EmitOnCompleted();
    }

protected:
    void OnDispose() noexcept override {
        DoDisposal();
    }

private:
    void OnDelayedWork(std::size_t id) {

        std::any value;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            // If the ID is not the last one, it means a new value has been emitted after this work
            // was scheduled, so this work should be ignored.
            if (id != last_id_) {
                return;
            }
            value = std::move(last_value_);
        }
        EmitOnNext(value);
    }

    void EmitLastValueOnCompleted() {
        std::any value;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            last_id_++;
            value = std::move(last_value_);
        }
        if (value.has_value()) {
            EmitOnNext(value);
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

    void CancelTimer() {
        auto sub = timer_.exchange(nullptr);
        if (sub) {
            sub->Dispose();
        }
    }

private:
    std::chrono::steady_clock::duration duration_{};
    std::shared_ptr<Scheduler> scheduler_;

    std::shared_ptr<Disposable> source_sub_;

    std::mutex mutex_;
    std::any last_value_;
    std::size_t last_id_{};
    std::atomic<std::shared_ptr<Disposable>> timer_;
};

} // namespace

DebounceOperator::DebounceOperator(
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


std::shared_ptr<Disposable> DebounceOperator::Subscribe(ObserverShim&& observer) {

    auto producer = std::make_shared<DebounceProducer>(
        std::move(observer),
        duration_,
        scheduler_);

    producer->Run(source_);
    return producer;
}

}