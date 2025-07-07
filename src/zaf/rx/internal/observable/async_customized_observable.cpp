#include <zaf/rx/internal/observable/async_customized_observable.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/base/error/error.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/observable/customized_observable.h>
#include <zaf/rx/internal/subscription/producer_subscription_core.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::rx::internal {
namespace {

class AsyncProducer : public Producer {
public:
    AsyncProducer(
        std::shared_ptr<SubscriptionCore> subscription,
        CancelTokenSource cancel_token_source)
        :
        Producer(nullptr),
        subscription_(subscription),
        cancel_token_source_(cancel_token_source) {

    }

protected:
    void OnUnsubscribe() override {

        subscription_->Unsubscribe();
        subscription_.reset();
        cancel_token_source_.Cancel();
    }

private:
    std::shared_ptr<SubscriptionCore> subscription_;
    CancelTokenSource cancel_token_source_;
};

}


AsyncCustomizedObservable::AsyncCustomizedObservable(
    std::shared_ptr<Scheduler> scheduler, 
    AsyncProcedure procedure) 
    :
    scheduler_(scheduler),
    procedure_(std::move(procedure)) {

    ZAF_EXPECT(scheduler_);
    ZAF_EXPECT(procedure_);
}


std::shared_ptr<SubscriptionCore> AsyncCustomizedObservable::Subscribe(
    const std::shared_ptr<ObserverCore>& observer) {

    CancelTokenSource cancel_token_source;

    auto nested_procedure = [procedure = procedure_, cancel_token_source](
        const std::shared_ptr<ObserverCore>& observer) {

        try {
            procedure(observer, cancel_token_source.GetToken());
        }
        catch (...) {
            observer->OnError(std::current_exception());
        }

        return nullptr;
    };

    auto nested_observable = std::make_shared<CustomizedObservable>(nested_procedure);
    auto nested_subscription = nested_observable->SubscribeOn(scheduler_)->Subscribe(observer);

    auto producer = std::make_shared<AsyncProducer>(
        nested_subscription, 
        cancel_token_source);

    return std::make_shared<ProducerSubscriptionCore>(std::move(producer));
}


}