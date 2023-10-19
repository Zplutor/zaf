#include <zaf/rx/internal/operator/observe_on_operator.h>
#include <zaf/base/as.h>
#include <zaf/rx/internal/inner_observer.h>
#include <zaf/rx/internal/subscription/inner_subscription.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::internal {
namespace {

class ObserveOnProducer : public Producer, public InnerObserver {
public:
    ObserveOnProducer(
        std::shared_ptr<InnerObserver> next_observer,
        std::shared_ptr<Scheduler> scheduler) 
        :
        Producer(std::move(next_observer)),
        scheduler_(std::move(scheduler)) { 
    
    }

    void Run(const std::shared_ptr<InnerObservable>& source) {
        source_subscription_ = source->Subscribe(As<ObserveOnProducer>(shared_from_this()));
    }

    void OnNext(const std::any& value) override {

        scheduler_->Schedule(std::bind(
            &ObserveOnProducer::OnNextOnScheduler,
            As<ObserveOnProducer>(shared_from_this()),
            value));
    }

    void OnError(const Error& error) override {

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
        source_subscription_->Unsubscribe();
        source_subscription_.reset();
    }

private:
    void OnNextOnScheduler(const std::any& value) {
        if (!is_unsubscribed_.load()) {
            DeliverOnNext(value);
        }
    }

    void OnErrorOnScheduler(const Error& error) {
        if (!is_unsubscribed_.load()) {
            DeliverOnError(error);
        }
    }

    void OnCompletedOnScheduler() {
        if (!is_unsubscribed_.load()) {
            DeliverOnCompleted();
        }
    }

private:
    std::shared_ptr<Scheduler> scheduler_;
    std::shared_ptr<InnerSubscription> source_subscription_;
    std::atomic<bool> is_unsubscribed_{ false };
};

}

ObserveOnOperator::ObserveOnOperator(
    std::shared_ptr<InnerObservable> source,
    std::shared_ptr<Scheduler> scheduler)
    :
    source_(std::move(source)),
    scheduler_(std::move(scheduler)) {

}


std::shared_ptr<InnerSubscription> ObserveOnOperator::Subscribe(
    const std::shared_ptr<InnerObserver>& observer) {

    auto producer = std::make_shared<ObserveOnProducer>(observer, scheduler_);
    producer->Run(source_);
    return std::make_shared<InnerSubscription>(producer);
}

}