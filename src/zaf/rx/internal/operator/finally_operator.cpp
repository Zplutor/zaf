#include <zaf/rx/internal/operator/finally_operator.h>
#include <zaf/base/as.h>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/producer.h>
#include <zaf/rx/internal/subscription/producer_subscription_core.h>

namespace zaf::rx::internal {
namespace {

class FinallyProducer : public Producer, public ObserverCore {
public:
    FinallyProducer(
        std::shared_ptr<ObserverCore> next_observer,
        Closure finally_work)
        :
        Producer(std::move(next_observer)),
        finally_work_(std::move(finally_work)) {

    }

    void Run(const std::shared_ptr<ObservableCore>& source) {
        source_subscription_ = source->Subscribe(As<ObserverCore>(shared_from_this()));
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

    void OnUnsubscribe() noexcept override {

        if (source_subscription_) {
            source_subscription_->Unsubscribe();
            source_subscription_.reset();
        }

        auto finally_work = std::move(finally_work_);
        finally_work();
    }

private:
    std::shared_ptr<SubscriptionCore> source_subscription_;
    Closure finally_work_;
};

}


FinallyOperator::FinallyOperator(std::shared_ptr<ObservableCore> source, Closure finally_work) :
    source_(std::move(source)),
    finally_work_(std::move(finally_work)) {

}


std::shared_ptr<SubscriptionCore> FinallyOperator::Subscribe(
    const std::shared_ptr<ObserverCore>& observer) {

    auto producer = std::make_shared<FinallyProducer>(observer, finally_work_);
    producer->Run(source_);
    return std::make_shared<ProducerSubscriptionCore>(std::move(producer));
}

}