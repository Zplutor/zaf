#include <zaf/rx/internal/operator/observe_on_operator.h>
#include <zaf/rx/internal/inner_observer.h>
#include <zaf/rx/internal/subscription/subscription_core.h>

namespace zaf::internal {
namespace {

class ObserveOnObserver : 
    public InnerObserver, 
    public SubscriptionCore, 
    public std::enable_shared_from_this<ObserveOnObserver> {

public:
    ObserveOnObserver(
        std::shared_ptr<InnerObserver> next_observer,
        std::shared_ptr<Scheduler> scheduler) 
        :
        SubscriptionCore(next_observer),
        scheduler_(std::move(scheduler)) { 
    
    }

    void AttachSourceSubscription(std::shared_ptr<SubscriptionCore> source_subscription) {
        source_subscription_ = std::move(source_subscription);
    }

    void OnNext(const std::any& value) override {

        scheduler_->Schedule(std::bind(
            &ObserveOnObserver::OnNextOnScheduler,
            shared_from_this(),
            value));
    }

    void OnError(const Error& error) override {

        scheduler_->Schedule(std::bind(
            &ObserveOnObserver::OnErrorOnScheduler, 
            shared_from_this(),
            error));
    }

    void OnCompleted() override {

        scheduler_->Schedule(std::bind(
            &ObserveOnObserver::OnCompletedOnScheduler, 
            shared_from_this()));
    }

protected:
    void OnUnsubscribe() override {

        is_unsubscribed_.store(true);
        source_subscription_->Unsubscribe();
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
    std::shared_ptr<SubscriptionCore> source_subscription_;
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


std::shared_ptr<SubscriptionCore> ObserveOnOperator::Subscribe(
    const std::shared_ptr<InnerObserver>& observer) {

    auto observe_on_observer = std::make_shared<ObserveOnObserver>(observer, scheduler_);
    auto source_subscription = source_->Subscribe(observe_on_observer);
    observe_on_observer->AttachSourceSubscription(source_subscription);
    return observe_on_observer;
}

}