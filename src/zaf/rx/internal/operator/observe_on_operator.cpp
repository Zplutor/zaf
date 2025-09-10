#include <zaf/rx/internal/operator/observe_on_operator.h>
#include <zaf/base/as.h>
#include <zaf/base/error/precondition_error.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::rx::internal {
namespace {

class ObserveOnProducer : public Producer, public ObserverCore {
public:
    ObserveOnProducer(
        ObserverShim&& next_observer,
        std::shared_ptr<Scheduler> scheduler) 
        :
        Producer(std::move(next_observer)),
        scheduler_(std::move(scheduler)) { 
    
    }

    ~ObserveOnProducer() {
        DoDisposal();
    }

    void Run(const std::shared_ptr<ObservableCore>& source) {
        source_subscription_ = source->Subscribe(
            ObserverShim::FromWeak(As<ObserverCore>(shared_from_this())));
    }

    void OnNext(const std::any& value) override {

        scheduler_->ScheduleWork(std::bind(
            &ObserveOnProducer::OnNextOnScheduler,
            As<ObserveOnProducer>(shared_from_this()),
            value));
    }

    void OnError(const std::exception_ptr& error) override {

        scheduler_->ScheduleWork(std::bind(
            &ObserveOnProducer::OnErrorOnScheduler, 
            As<ObserveOnProducer>(shared_from_this()),
            error));
    }

    void OnCompleted() override {

        scheduler_->ScheduleWork(std::bind(
            &ObserveOnProducer::OnCompletedOnScheduler, 
            As<ObserveOnProducer>(shared_from_this())));
    }

protected:
    void OnDispose() noexcept override {
        DoDisposal();
    }

private:
    void OnNextOnScheduler(const std::any& value) {
        if (!is_unsubscribed_.load()) {
            EmitOnNext(value);
        }
    }

    void OnErrorOnScheduler(const std::exception_ptr& error) {
        if (!is_unsubscribed_.load()) {
            EmitOnError(error);
        }
    }

    void OnCompletedOnScheduler() {
        if (!is_unsubscribed_.load()) {
            EmitOnCompleted();
        }
    }

    void DoDisposal() noexcept {
        is_unsubscribed_.store(true);
        if (source_subscription_) {
            source_subscription_->Dispose();
            source_subscription_.reset();
        }
    }

private:
    std::shared_ptr<Scheduler> scheduler_;
    std::shared_ptr<Disposable> source_subscription_;
    std::atomic<bool> is_unsubscribed_{ false };
};

}

ObserveOnOperator::ObserveOnOperator(
    std::shared_ptr<ObservableCore> source,
    std::shared_ptr<Scheduler> scheduler)
    :
    source_(std::move(source)),
    scheduler_(std::move(scheduler)) {

    ZAF_EXPECT(source_);
    ZAF_EXPECT(scheduler_);
}


std::shared_ptr<Disposable> ObserveOnOperator::Subscribe(ObserverShim&& observer) {

    auto producer = std::make_shared<ObserveOnProducer>(std::move(observer), scheduler_);
    producer->Run(source_);
    return producer;
}

}