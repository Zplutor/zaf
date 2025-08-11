#include <zaf/rx/internal/operator/do_on_terminate_operator.h>
#include <zaf/base/as.h>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/producer.h>
#include <zaf/rx/internal/subscription/producer_subscription_core.h>

namespace zaf::rx::internal {
namespace {

class DoOnTerminateProducer : public Producer, public ObserverCore {
public:
    DoOnTerminateProducer(
        std::shared_ptr<ObserverCore> next_observer,
        Closure on_terminate)
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
        try {
            // The subscription may be unsubscribed in the on_terminate_() call.
            on_terminate_();
        }
        catch (...) {
            // Exception thrown in the on_terminate_() call should be propagated to the downstream 
            // observer.
            EmitOnError(std::current_exception());
            return;
        }
        EmitOnError(error);
    }

    void OnCompleted() override {
        try {
            // The subscription may be unsubscribed in the on_terminate_() call.
            on_terminate_();
        }
        catch (...) {
            // Exception thrown in the on_terminate_() call should be propagated to the downstream 
            // observer.
            EmitOnError(std::current_exception());
            return;
        }
        EmitOnCompleted();
    }

    void OnDispose() noexcept override {

        if (source_subscription_) {
            source_subscription_->Dispose();
            source_subscription_.reset();
        }
        on_terminate_ = nullptr;
    }

private:
    std::shared_ptr<SubscriptionCore> source_subscription_;
    Closure on_terminate_;
};

}


DoOnTerminateOperator::DoOnTerminateOperator(
    std::shared_ptr<ObservableCore> source, 
    Closure on_terminate)
    :
    source_(std::move(source)),
    on_terminate_(std::move(on_terminate)) {

}


std::shared_ptr<SubscriptionCore> DoOnTerminateOperator::Subscribe(
    const std::shared_ptr<ObserverCore>& observer) {

    auto producer = std::make_shared<DoOnTerminateProducer>(observer, on_terminate_);
    producer->Run(source_);
    return std::make_shared<ProducerSubscriptionCore>(std::move(producer));
}

}