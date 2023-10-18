#include <zaf/rx/internal/observable/async_customized_observable.h>
#include <zaf/base/error/check.h>
#include <zaf/base/error/error.h>
#include <zaf/rx/internal/inner_observer.h>
#include <zaf/rx/internal/observable/customized_observable.h>
#include <zaf/rx/internal/subscription/inner_subscription.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::internal {
namespace {

class AsyncProducer : public Producer {
public:
    AsyncProducer(
        std::shared_ptr<InnerSubscription> subscription,
        CancelTokenSource cancel_token_source)
        :
        Producer(nullptr),
        subscription_(subscription),
        cancel_token_source_(cancel_token_source) {

    }

protected:
    void OnDispose() override {

        subscription_->Unsubscribe();
        cancel_token_source_.Cancel();
    }

private:
    std::shared_ptr<InnerSubscription> subscription_;
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


std::shared_ptr<InnerSubscription> AsyncCustomizedObservable::Subscribe(
    const std::shared_ptr<InnerObserver>& observer) {

    CancelTokenSource cancel_token_source;

    auto nested_procedure = [procedure = procedure_, cancel_token_source](
        const std::shared_ptr<InnerObserver>& observer) {

        try {

            procedure(observer, cancel_token_source.GetToken());
        }
        catch (const Error& error) {
            observer->OnError(error);
        }

        return InnerSubscription::Empty();
    };

    auto nested_observable = std::make_shared<CustomizedObservable>(nested_procedure);
    auto nested_subscription = nested_observable->SubscribeOn(scheduler_)->Subscribe(observer);

    auto producer = std::make_shared<AsyncProducer>(
        nested_subscription, 
        cancel_token_source);

    return std::make_shared<InnerSubscription>(producer);
}


}