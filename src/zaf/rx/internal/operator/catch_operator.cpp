#include <zaf/rx/internal/operator/catch_operator.h>
#include <zaf/base/as.h>
#include <zaf/rx/internal/producer.h>
#include <zaf/rx/internal/subscription/producer_subscription_core.h>

namespace zaf::rx::internal {
namespace {

class CatchProducer : public Producer, public ObserverCore {
public:
    CatchProducer(
        std::shared_ptr<ObserverCore> next_observer,
        CatchHandler handler)
        :
        Producer(std::move(next_observer)),
        handler_(std::move(handler)) {

    }

    void Run(const std::shared_ptr<ObservableCore>& source) {
        source_subscription_ = source->Subscribe(As<ObserverCore>(shared_from_this()));
    }

    void OnNext(const std::any& value) override {
        EmitOnNext(value);
    }

    void OnError(const std::exception_ptr& error) override {

        std::shared_ptr<ObservableCore> new_observable;
        try {
            new_observable = handler_(error);
        }
        catch (...) {
            EmitOnError(std::current_exception());
            return;
        }

        UnsubscribeSource();
        SubscribeToNewObservable(new_observable);
    }

    void OnCompleted() override {
        EmitOnCompleted();
    }

protected:
    void OnUnsubscribe() noexcept override {

        UnsubscribeSource();

        if (new_subscription_) {
            new_subscription_->Unsubscribe();
            new_subscription_.reset();
        }

        handler_ = nullptr;
    }

private:
    void SubscribeToNewObservable(const std::shared_ptr<ObservableCore>& observable) {

        //New observable might emit and complete immediately on subscribe, and the current producer 
        //would be freed immediately as well. Thus, subsequent access to member fields would fail. 
        //We use a weak pointer to check if the current producer has been freed to avoid this 
        //issue.
        auto weak_this = weak_from_this();
        auto new_subscription = observable->Subscribe(ObserverCore::Create(
            [this](const std::any& value) {
                EmitOnNext(value);
            },
            [this](const std::exception_ptr& error) {
                EmitOnError(error);
            },
            [this]() {
                EmitOnCompleted();
            }
        ));

        auto shared_this = weak_this.lock();
        if (shared_this) {
            new_subscription_ = std::move(new_subscription);
        }
    }

    void UnsubscribeSource() {

        if (source_subscription_) {
            source_subscription_->Unsubscribe();
            source_subscription_.reset();
        }
    }

private:
    std::shared_ptr<SubscriptionCore> source_subscription_;
    CatchHandler handler_;
    std::shared_ptr<SubscriptionCore> new_subscription_;
};

}

CatchOperator::CatchOperator(std::shared_ptr<ObservableCore> source, CatchHandler handler) :
    source_(std::move(source)),
    handler_(std::move(handler)) {

}


std::shared_ptr<SubscriptionCore> CatchOperator::Subscribe(
    const std::shared_ptr<ObserverCore>& observer) {

    auto producer = std::make_shared<CatchProducer>(observer, handler_);
    producer->Run(source_);
    return std::make_shared<ProducerSubscriptionCore>(std::move(producer));
}

}