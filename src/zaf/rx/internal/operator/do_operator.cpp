#include <zaf/rx/internal/operator/do_operator.h>
#include <zaf/base/as.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/subscription/subscription_core.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::rx::internal {
namespace {

class DoProducer : public Producer, public ObserverCore {
public:
    DoProducer(
        std::shared_ptr<ObserverCore> next_observer,
        std::shared_ptr<ObserverCore> do_observer)
        :
        Producer(std::move(next_observer)),
        do_observer_(std::move(do_observer)) {

    }

    void Run(const std::shared_ptr<ObservableCore>& source) {
        source_subscription_ = source->Subscribe(As<ObserverCore>(shared_from_this()));
    }

    void OnNext(const std::any& value) override {
        do_observer_->OnNext(value);
        EmitOnNext(value);
    }

    void OnError(const std::exception_ptr& error) override {
        do_observer_->OnError(error);
        EmitOnError(error);
    }

    void OnCompleted() override {
        do_observer_->OnCompleted();
        EmitOnCompleted();
    }

    void OnDispose() override {
        source_subscription_->Unsubscribe();
        source_subscription_.reset();
        do_observer_.reset();
    }

private:
    std::shared_ptr<SubscriptionCore> source_subscription_;
    std::shared_ptr<ObserverCore> do_observer_;
};

}

DoOperator::DoOperator(
    std::shared_ptr<ObservableCore> source,
    std::shared_ptr<ObserverCore> do_observer)
    :
    source_(std::move(source)),
    do_observer_(std::move(do_observer)) {

}


std::shared_ptr<SubscriptionCore> DoOperator::Subscribe(
    const std::shared_ptr<ObserverCore>& observer) {

    auto producer = std::make_shared<DoProducer>(observer, do_observer_);
    producer->Run(source_);
    return std::make_shared<SubscriptionCore>(std::move(producer));
}

}