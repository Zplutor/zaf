#include <zaf/rx/internal/operator/catch_operator.h>
#include <zaf/base/as.h>
#include <zaf/rx/internal/producer.h>
#include <zaf/rx/internal/subscription/inner_subscription.h>

namespace zaf::internal {
namespace {

class CatchProducer : public Producer, public InnerObserver {
public:
    CatchProducer(
        std::shared_ptr<InnerObserver> next_observer,
        CatchHandler handler)
        :
        Producer(std::move(next_observer)),
        handler_(std::move(handler)) {

    }

    void Run(const std::shared_ptr<InnerObservable>& source) {
        source_subscription_ = source->Subscribe(As<InnerObserver>(shared_from_this()));
    }

    void OnNext(const std::any& value) override {
        DeliverOnNext(value);
    }

    void OnError(const Error& error) override {

        std::shared_ptr<InnerObservable> new_observable;
        try {
            new_observable = handler_(error);
        }
        catch (const zaf::Error& error) {
            DeliverOnError(error);
            return;
        }

        UnsubscribeSource();
        SubscribeToNewObservable(new_observable);
    }

    void OnCompleted() override {
        DeliverOnCompleted();
    }

protected:
    void OnDispose() override {

        UnsubscribeSource();

        if (new_subscription_) {
            new_subscription_->Unsubscribe();
            new_subscription_.reset();
        }

        handler_ = nullptr;
    }

private:
    void SubscribeToNewObservable(const std::shared_ptr<InnerObservable>& observable) {

        //New observable might emit and complete immediately on subscribe, and the current producer 
        //would be freed immediately as well. Thus, subsequent access to member fields would fail. 
        //We use a weak pointer to check if the current producer has been freed to avoid this 
        //issue.
        auto weak_this = weak_from_this();
        auto new_subscription = observable->Subscribe(InnerObserver::Create(
            [this](const std::any& value) {
                DeliverOnNext(value);
            },
            [this](const zaf::Error& error) {
                DeliverOnError(error);
            },
            [this]() {
                DeliverOnCompleted();
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
    std::shared_ptr<InnerSubscription> source_subscription_;
    CatchHandler handler_;
    std::shared_ptr<InnerSubscription> new_subscription_;
};

}

CatchOperator::CatchOperator(std::shared_ptr<InnerObservable> source, CatchHandler handler) :
    source_(std::move(source)),
    handler_(std::move(handler)) {

}


std::shared_ptr<InnerSubscription> CatchOperator::Subscribe(
    const std::shared_ptr<InnerObserver>& observer) {

    auto producer = std::make_shared<CatchProducer>(observer, handler_);
    producer->Run(source_);
    return std::make_shared<InnerSubscription>(std::move(producer));
}

}