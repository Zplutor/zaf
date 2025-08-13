#include <zaf/rx/internal/operator/subscribe_on_operator.h>
#include <zaf/base/as.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/rx/internal/subscription/producer_subscription_core.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::rx::internal {
namespace {

class SubscribeOnProducer : public Producer, public ObserverCore {
public:
    SubscribeOnProducer(
        std::shared_ptr<ObservableCore> source,
        std::shared_ptr<Scheduler> scheduler,
        ObserverShim&& observer) 
        :
        Producer(std::move(observer)),
        source_(std::move(source)),
        scheduler_(std::move(scheduler)) {
    
    }

    ~SubscribeOnProducer() {

    }

    void Run() {

        scheduler_->ScheduleWork(std::bind(
            &SubscribeOnProducer::SubscribeOnScheduler, 
            As<SubscribeOnProducer>(shared_from_this())));
    }

    void OnDispose() noexcept override {

        is_disposed_.store(true);

        scheduler_->ScheduleWork(std::bind(
            &SubscribeOnProducer::UnsubscribeOnScheduler,
            As<SubscribeOnProducer>(shared_from_this())));
    }

    void OnNext(const std::any& value) override {
        EmitOnNext(value);
    }

    void OnError(const std::exception_ptr& error) override {
        EmitOnError(error);
    }

    void OnCompleted() override {
        EmitOnCompleted();
    }

private:
    void SubscribeOnScheduler() {

        if (is_disposed_.load()) {
            return;
        }

        source_subscription_ = source_->Subscribe(
            ObserverShim::FromWeak(As<SubscribeOnProducer>(shared_from_this())));
    }

    void UnsubscribeOnScheduler() {

        if (source_subscription_) {
            source_subscription_->Dispose();
            source_subscription_.reset();
        }

        source_.reset();
    }

private:
    std::shared_ptr<ObservableCore> source_;
    std::shared_ptr<Scheduler> scheduler_;
    std::shared_ptr<SubscriptionCore> source_subscription_;
    std::atomic<bool> is_disposed_{};
};

}

SubscribeOnOperator::SubscribeOnOperator(
    std::shared_ptr<ObservableCore> source,
    std::shared_ptr<Scheduler> scheduler) 
    :
    source_(std::move(source)),
    scheduler_(std::move(scheduler)) {

    ZAF_EXPECT(source_);
    ZAF_EXPECT(scheduler_);
}


std::shared_ptr<SubscriptionCore> SubscribeOnOperator::Subscribe(ObserverShim&& observer) {

    auto producer = std::make_shared<SubscribeOnProducer>(
        source_, 
        scheduler_, 
        std::move(observer));

    producer->Run();
    return std::make_shared<ProducerSubscriptionCore>(std::move(producer));
}

}