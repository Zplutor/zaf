#include <zaf/rx/internal/observable/custom_observable.h>
#include <zaf/base/as.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::rx::internal {
namespace {

class CustomProducer : public Producer, public ObserverCore {
public:
    explicit CustomProducer(ObserverShim&& observer) : Producer(std::move(observer)) {

    }

    void EmitCaughtError(const std::exception_ptr& error) {
        InnerEmitOnError(error, true);
    }

    void OnNext(const std::any& value) override {
        this->EmitOnNext(value);
    }

    void OnError(const std::exception_ptr& error) override {
        InnerEmitOnError(error, false);
    }

    void OnCompleted() override {
        this->EmitOnCompleted();
    }

private:
    void InnerEmitOnError(const std::exception_ptr& error, bool is_caught_error) {
        if (!this->EmitOnError(error)) {
            if (is_caught_error) {
                std::rethrow_exception(error);
            }
        }
    }
};

}

CustomObservable::CustomObservable(Procedure procedure) : 
    procedure_(std::move(procedure)) {

}


std::shared_ptr<SubscriptionCore> CustomObservable::Subscribe(ObserverShim&& observer) {

    auto producer = std::make_shared<CustomProducer>(std::move(observer));
    try {
        procedure_(producer, producer);
    }
    catch (...) {
        producer->EmitCaughtError(std::current_exception());
    }
    return producer;
}

}