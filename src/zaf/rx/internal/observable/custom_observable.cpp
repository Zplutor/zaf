#include <zaf/rx/internal/observable/custom_observable.h>
#include <zaf/base/as.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/producer.h>
#include <zaf/rx/internal/subscription/producer_subscription_core.h>

namespace zaf::rx::internal {
namespace {

class CustomProducer : public Producer, public ObserverCore {
public:
    CustomProducer(std::shared_ptr<ObserverCore> observer) :
        Producer(std::move(observer)) {

    }

    void OnNext(const std::any& value) override {
        this->EmitOnNext(value);
    }

    void OnError(const std::exception_ptr& error) override {
        this->EmitOnError(error);
    }

    void OnCompleted() override {
        this->EmitOnCompleted();
    }

protected:
    void OnUnsubscribe() override {

    }
};

}

CustomObservable::CustomObservable(Procedure procedure) : 
    procedure_(std::move(procedure)) {

}


std::shared_ptr<SubscriptionCore> CustomObservable::Subscribe(
    const std::shared_ptr<ObserverCore>& observer) {

    auto producer = std::make_shared<CustomProducer>(observer);
    auto subscription_core = std::make_shared<ProducerSubscriptionCore>(producer);

    try {
        procedure_(producer, subscription_core);
    }
    catch (...) {
        producer->OnError(std::current_exception());
    }
    return subscription_core;
}

}