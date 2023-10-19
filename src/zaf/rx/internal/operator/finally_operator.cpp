#include <zaf/rx/internal/operator/finally_operator.h>
#include <zaf/base/as.h>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/internal/inner_observer.h>
#include <zaf/rx/internal/producer.h>
#include <zaf/rx/internal/subscription/inner_subscription.h>

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

    void Run(const std::shared_ptr<InnerObservable>& source) {
        source_subscription_ = source->Subscribe(As<InnerObserver>(shared_from_this()));
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
        source_subscription_.reset();

        if (finally_work_) {
            finally_work_();
            finally_work_ = nullptr;
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
    producer->Run(source_);
    return std::make_shared<InnerSubscription>(std::move(producer));
}

}