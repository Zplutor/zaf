#include <zaf/rx/internal/operator/finally_operator.h>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/internal/inner_observer.h>
#include <zaf/rx/internal/subscription/inner_subscription.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::internal {
namespace {

class FinallyProducer : public Producer, public InnerObserver {
public:
    FinallyProducer(
        std::shared_ptr<InnerObserver> next_observer,
        Work finally_work)
        :
        Producer(std::move(next_observer)),
        finally_work_(std::move(finally_work)) {

    }

    void AttachSourceSubscription(std::shared_ptr<InnerSubscription> source_subscription) {
        source_subscription_ = std::move(source_subscription);
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

    void OnDispose() override {

        source_subscription_->Unsubscribe();

        if (finally_work_) {
            finally_work_();
        }
    }

private:
    std::shared_ptr<InnerSubscription> source_subscription_;
    Work finally_work_;
};

}


FinallyOperator::FinallyOperator(std::shared_ptr<InnerObservable> source, Work finally_work) :
    source_(std::move(source)),
    finally_work_(std::move(finally_work)) {

}


std::shared_ptr<InnerSubscription> FinallyOperator::Subscribe(
    const std::shared_ptr<InnerObserver>& observer) {

    auto producer = std::make_shared<FinallyProducer>(observer, std::move(finally_work_));

    auto source_subscription = source_->Subscribe(producer);
    producer->AttachSourceSubscription(std::move(source_subscription));

    return std::make_shared<InnerSubscription>(std::move(producer));
}

}