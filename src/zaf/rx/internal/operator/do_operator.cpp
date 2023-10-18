#include <zaf/rx/internal/operator/do_operator.h>
#include <zaf/rx/internal/inner_observer.h>
#include <zaf/rx/internal/subscription/inner_subscription.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::internal {
namespace {

class DoProducer : public Producer, public InnerObserver {
public:
    DoProducer(
        std::shared_ptr<InnerObserver> next_observer,
        std::shared_ptr<InnerObserver> do_observer)
        :
        Producer(std::move(next_observer)),
        do_observer_(std::move(do_observer)) {

    }

    void AttachSourceSubscription(std::shared_ptr<InnerSubscription> source_subscription) {
        source_subscription_ = std::move(source_subscription);
    }

    void OnNext(const std::any& value) override {
        do_observer_->OnNext(value);
        DeliverOnNext(value);
    }

    void OnError(const Error& error) override {
        do_observer_->OnError(error);
        DeliverOnError(error);
    }

    void OnCompleted() override {
        do_observer_->OnCompleted();
        DeliverOnCompleted();
    }

    void OnDispose() override {
        source_subscription_->Unsubscribe();
    }

private:
    std::shared_ptr<InnerSubscription> source_subscription_;
    std::shared_ptr<InnerObserver> do_observer_;
};

}

DoOperator::DoOperator(
    std::shared_ptr<InnerObservable> source,
    std::shared_ptr<InnerObserver> do_observer)
    :
    source_(std::move(source)),
    do_observer_(std::move(do_observer)) {

}


std::shared_ptr<InnerSubscription> DoOperator::Subscribe(
    const std::shared_ptr<InnerObserver>& observer) {

    auto producer = std::make_shared<DoProducer>(observer, std::move(do_observer_));

    auto source_subscription = source_->Subscribe(producer);
    producer->AttachSourceSubscription(std::move(source_subscription));

    return std::make_shared<InnerSubscription>(std::move(producer));
}

}