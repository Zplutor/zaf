#include <zaf/rx/internal/operator/observe_on_operator.h>
#include <zaf/base/as.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/subscription/subscription_core.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::rx::internal {
namespace {

class ObserveOnProducer : public Producer, public ObserverCore {
public:
    ObserveOnProducer(
        std::shared_ptr<ObserverCore> next_observer,
        std::shared_ptr<Scheduler> scheduler) 
        :
        Producer(std::move(next_observer)),
        scheduler_(std::move(scheduler)) { 
    
    }

    void Run(const std::shared_ptr<ObservableCore>& source) {
        source_subscription_ = source->Subscribe(As<ObserveOnProducer>(shared_from_this()));
    }

    void OnNext(const std::any& value) override {

        scheduler_->Schedule(std::bind(
            &ObserveOnProducer::OnNextOnScheduler,
            As<ObserveOnProducer>(shared_from_this()),
            value));
    }

    void OnError(const std::exception_ptr& error) override {

        scheduler_->Schedule(std::bind(
            &ObserveOnProducer::OnErrorOnScheduler, 
            As<ObserveOnProducer>(shared_from_this()),
            error));
    }

    void OnCompleted() override {

        scheduler_->Schedule(std::bind(
            &ObserveOnProducer::OnCompletedOnScheduler, 
            As<ObserveOnProducer>(shared_from_this())));
    }

protected:
    void OnDispose() override {

        is_unsubscribed_.store(true);
        if (source_subscription_) {
            source_subscription_->Unsubscribe();
            source_subscription_.reset();
        }
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

private:
    std::shared_ptr<Scheduler> scheduler_;
    std::shared_ptr<SubscriptionCore> source_subscription_;
    std::atomic<bool> is_unsubscribed_{ false };
};

}

ObserveOnOperator::ObserveOnOperator(
    std::shared_ptr<ObservableCore> source,
    std::shared_ptr<Scheduler> scheduler)
    :
    source_(std::move(source)),
    scheduler_(std::move(scheduler)) {

}


std::shared_ptr<SubscriptionCore> ObserveOnOperator::Subscribe(
    const std::shared_ptr<ObserverCore>& observer) {

    auto producer = std::make_shared<ObserveOnProducer>(observer, scheduler_);
    producer->Run(source_);
    return std::make_shared<SubscriptionCore>(producer);
}

}