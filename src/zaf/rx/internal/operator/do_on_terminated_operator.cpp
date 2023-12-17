#include <zaf/rx/internal/operator/do_on_terminated_operator.h>
#include <zaf/base/as.h>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/internal/inner_observer.h>
#include <zaf/rx/internal/producer.h>
#include <zaf/rx/internal/subscription/inner_subscription.h>

namespace zaf::internal {
namespace {

class DoOnTerminatedProducer : public Producer, public InnerObserver {
public:
    DoOnTerminatedProducer(
        std::shared_ptr<InnerObserver> next_observer,
        Work on_terminated)
        :
        Producer(std::move(next_observer)),
        on_terminated_(std::move(on_terminated)) {

    }

    void Run(const std::shared_ptr<InnerObservable>& source) {
        source_subscription_ = source->Subscribe(As<InnerObserver>(shared_from_this()));
    }

    void OnNext(const std::any& value) override {
        EmitOnNext(value);
    }

    void OnError(const Error& error) override {
        on_terminated_();
        EmitOnError(error);
    }

    void OnCompleted() override {
        on_terminated_();
        EmitOnCompleted();
    }

    void OnDispose() override {

        source_subscription_->Unsubscribe();
        source_subscription_.reset();
        on_terminated_ = nullptr;
    }

private:
    std::shared_ptr<InnerSubscription> source_subscription_;
    Work on_terminated_;
};

}


DoOnTerminatedOperator::DoOnTerminatedOperator(
    std::shared_ptr<InnerObservable> source, 
    Work on_terminated)
    :
    source_(std::move(source)),
    on_terminated_(std::move(on_terminated)) {

}


std::shared_ptr<InnerSubscription> DoOnTerminatedOperator::Subscribe(
    const std::shared_ptr<InnerObserver>& observer) {

    auto producer = std::make_shared<DoOnTerminatedProducer>(observer, on_terminated_);
    producer->Run(source_);
    return std::make_shared<InnerSubscription>(std::move(producer));
}

}