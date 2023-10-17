#include <zaf/rx/internal/operator/finally_operator.h>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/internal/inner_observer.h>
#include <zaf/rx/internal/subscription/inner_subscription.h>
#include <zaf/rx/internal/subscription/subscription_core.h>

namespace zaf::internal {
namespace {

class FinallyCore : public InnerObserver, public SubscriptionCore {
public:
    FinallyCore(
        std::shared_ptr<InnerObserver> next_observer,
        Work finally_work)
        :
        SubscriptionCore(std::move(next_observer)),
        finally_work_(std::move(finally_work)) {

    }

    void AttachSourceSubscription(std::shared_ptr<InnerSubscription> source_subscription) {
        source_subscription_ = std::move(source_subscription);
    }

    void OnNext(const std::any& value) override {
        DeliverOnNext(value);
    }

    void OnError(const Error& error) override {

        FinallyCaller caller(std::move(finally_work_));
        DeliverOnError(error);
    }

    void OnCompleted() override {

        FinallyCaller caller(std::move(finally_work_));
        DeliverOnCompleted();
    }

    void OnUnsubscribe() override {

        source_subscription_->Unsubscribe();

        if (finally_work_) {
            finally_work_();
        }
    }

private:
    class FinallyCaller  : zaf::NonCopyableNonMovable {
    public:
        explicit FinallyCaller(Work&& work) : work_(std::move(work)) {

        }

        ~FinallyCaller() {
            work_();
        }

    private:
        Work work_;
    };

private:
    std::shared_ptr<InnerSubscription> source_subscription_;
    Work finally_work_;
};

}


FinallyOperator::FinallyOperator(std::shared_ptr<InnerObservable> source, Work finally_work) :
    source_(std::move(source)),
    finally_work_(std::move(finally_work)) {

}


std::shared_ptr<InnerSubscription> FinallyOperator::Subscribe(
    const std::shared_ptr<InnerObserver>& observer) {

    auto finally_core = std::make_shared<FinallyCore>(observer, std::move(finally_work_));

    auto source_subscription = source_->Subscribe(finally_core);
    finally_core->AttachSourceSubscription(std::move(source_subscription));

    return std::make_shared<InnerSubscription>(std::move(finally_core));
}

}