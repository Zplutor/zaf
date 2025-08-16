#include <zaf/rx/internal/operator/finally_operator.h>
#include <zaf/base/as.h>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::rx::internal {
namespace {

class FinallyProducer : public Producer, public ObserverCore {
public:
    FinallyProducer(
        ObserverShim&& next_observer,
        Closure finally_work)
        :
        Producer(std::move(next_observer)),
        finally_work_(std::move(finally_work)) {

    }

    ~FinallyProducer() {
        DoDisposal();
    }

    void Run(const std::shared_ptr<ObservableCore>& source) {
        source_subscription_ = source->Subscribe(
            ObserverShim::FromWeak(As<ObserverCore>(shared_from_this())));
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

    void OnDispose() noexcept override {
        DoDisposal();
    }

private:
    void DoDisposal() noexcept {

        if (source_subscription_) {
            source_subscription_->Dispose();
            source_subscription_.reset();
        }

        if (finally_work_) {
            auto finally_work = std::move(finally_work_);
            finally_work();
        }
    }

private:
    std::shared_ptr<Disposable> source_subscription_;
    Closure finally_work_;
};

}


FinallyOperator::FinallyOperator(std::shared_ptr<ObservableCore> source, Closure finally_work) :
    source_(std::move(source)),
    finally_work_(std::move(finally_work)) {

}


std::shared_ptr<Disposable> FinallyOperator::Subscribe(ObserverShim&& observer) {

    auto producer = std::make_shared<FinallyProducer>(std::move(observer), finally_work_);
    producer->Run(source_);
    return producer;
}

}