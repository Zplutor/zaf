#include <zaf/rx/internal/operator/do_on_terminate_operator.h>
#include <zaf/base/as.h>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/producer.h>
#include <zaf/rx/internal/subscription/subscription_core.h>

namespace zaf::rx::internal {
namespace {

class DoOnTerminateProducer : public Producer, public ObserverCore {
public:
    DoOnTerminateProducer(
        std::shared_ptr<ObserverCore> next_observer,
        Work on_terminate)
        :
        Producer(std::move(next_observer)),
        on_terminate_(std::move(on_terminate)) {

    }

    void Run(const std::shared_ptr<ObservableCore>& source) {
        source_subscription_ = source->Subscribe(As<ObserverCore>(shared_from_this()));
    }

    void OnNext(const std::any& value) override {
        EmitOnNext(value);
    }

    void OnError(const std::exception_ptr& error) override {
        on_terminate_();
        EmitOnError(error);
    }

    void OnCompleted() override {
        on_terminate_();
        EmitOnCompleted();
    }

    void OnDispose() override {

        if (source_subscription_) {
            source_subscription_->Unsubscribe();
            source_subscription_.reset();
        }
        on_terminate_ = nullptr;
    }

private:
    std::shared_ptr<SubscriptionCore> source_subscription_;
    Work on_terminate_;
};

}


DoOnTerminateOperator::DoOnTerminateOperator(
    std::shared_ptr<ObservableCore> source, 
    Work on_terminate)
    :
    source_(std::move(source)),
    on_terminate_(std::move(on_terminate)) {

}


std::shared_ptr<SubscriptionCore> DoOnTerminateOperator::Subscribe(
    const std::shared_ptr<ObserverCore>& observer) {

    auto producer = std::make_shared<DoOnTerminateProducer>(observer, on_terminate_);
    producer->Run(source_);
    return std::make_shared<SubscriptionCore>(std::move(producer));
}

}