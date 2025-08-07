#include <zaf/rx/internal/operator/do_after_terminate_operator.h>
#include <zaf/base/as.h>
#include <zaf/base/auto_reset.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/producer.h>
#include <zaf/rx/internal/subscription/producer_subscription_core.h>

namespace zaf::rx::internal {
namespace {

class DoAfterTerminateProducer : public Producer, public ObserverCore {
public:
    DoAfterTerminateProducer(
        std::shared_ptr<ObserverCore> next_observer,
        Closure after_terminate)
        :
        Producer(std::move(next_observer)),
        after_terminate_(std::move(after_terminate)) {

    }

    void Run(const std::shared_ptr<ObservableCore>& source) {
        source_subscription_ = source->Subscribe(As<ObserverCore>(shared_from_this()));
    }

    void OnNext(const std::any& value) override {
        EmitOnNext(value);
    }

    void OnError(const std::exception_ptr& error) override {
        auto auto_reset = MakeAutoReset(is_emitting_termination_, true);
        EmitOnError(error);
    }

    void OnCompleted() override {
        auto auto_reset = MakeAutoReset(is_emitting_termination_, true);
        EmitOnCompleted();
    }

    void OnDispose() noexcept override {

        if (is_emitting_termination_) {
            after_terminate_();
        }

        if (source_subscription_) {
            source_subscription_->Dispose();
            source_subscription_.reset();
        }
        after_terminate_ = nullptr;
    }

private:
    std::shared_ptr<SubscriptionCore> source_subscription_;
    Closure after_terminate_;
    bool is_emitting_termination_{};
};

} // namespace


DoAfterTerminateOperator::DoAfterTerminateOperator(
    std::shared_ptr<ObservableCore> source,
    Closure after_terminate)
    : 
    source_(std::move(source)),
    after_terminate_(std::move(after_terminate)) {

}


std::shared_ptr<SubscriptionCore> DoAfterTerminateOperator::Subscribe(
    const std::shared_ptr<ObserverCore>& observer) {

    auto producer = std::make_shared<DoAfterTerminateProducer>(observer, after_terminate_);
    producer->Run(source_);
    return std::make_shared<ProducerSubscriptionCore>(std::move(producer));
}

}