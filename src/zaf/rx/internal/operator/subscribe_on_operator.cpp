#include <zaf/rx/internal/operator/subscribe_on_operator.h>
#include <zaf/base/error/check.h>
#include <zaf/rx/internal/subscription/inner_subscription.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::internal {
namespace {

class SubscribeOnProducer : 
    public Producer, 
    public InnerObserver,
    public std::enable_shared_from_this<SubscribeOnProducer> {

public:
    SubscribeOnProducer(
        std::shared_ptr<InnerObservable> source,
        std::shared_ptr<Scheduler> scheduler,
        std::shared_ptr<InnerObserver> observer) 
        :
        Producer(std::move(observer)),
        source_(std::move(source)),
        scheduler_(std::move(scheduler)) {
    
    }

    ~SubscribeOnProducer() {

    }

    void Subscribe() {

        scheduler_->Schedule(std::bind(
            &SubscribeOnProducer::SubscribeOnScheduler, 
            shared_from_this()));
    }

    void OnUnsubscribe() override {

        is_unsubscribed_.store(true);

        scheduler_->Schedule(std::bind(
            &SubscribeOnProducer::UnsubscribeOnScheduler,
            shared_from_this()));
    }

    void OnNext(const std::any& value) override {
        DeliverOnNext(value);
    }

    void OnError(const Error& error) override {
        DeliverOnError(error);
    }

    void OnCompleted() override {
        DeliverOnCompleted();
    }

private:
    void SubscribeOnScheduler() {

        if (is_unsubscribed_.load()) {
            return;
        }

        source_subscription_ = source_->Subscribe(shared_from_this());
    }

    void UnsubscribeOnScheduler() {

        if (source_subscription_) {
            source_subscription_->Unsubscribe();
        }
    }

private:
    std::shared_ptr<InnerObservable> source_;
    std::shared_ptr<Scheduler> scheduler_;
    std::shared_ptr<InnerSubscription> source_subscription_;
    std::atomic<bool> is_unsubscribed_{};
};

}

SubscribeOnOperator::SubscribeOnOperator(
    std::shared_ptr<InnerObservable> source,
    std::shared_ptr<Scheduler> scheduler) 
    :
    source_(std::move(source)),
    scheduler_(std::move(scheduler)) {

    ZAF_EXPECT(source_);
    ZAF_EXPECT(scheduler_);
}


std::shared_ptr<InnerSubscription> SubscribeOnOperator::Subscribe(
    const std::shared_ptr<InnerObserver>& observer) {

    ZAF_EXPECT(observer);

    auto producer = std::make_shared<SubscribeOnProducer>(
        source_, 
        scheduler_, 
        observer);

    producer->Subscribe();

    return std::make_shared<InnerSubscription>(producer);
}

}