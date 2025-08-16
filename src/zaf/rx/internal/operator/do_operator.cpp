#include <zaf/rx/internal/operator/do_operator.h>
#include <zaf/base/as.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::rx::internal {
namespace {

class DoProducer : public Producer, public ObserverCore {
public:
    DoProducer(
        ObserverShim&& next_observer,
        std::shared_ptr<ObserverCore> do_observer)
        :
        Producer(std::move(next_observer)),
        do_observer_(std::move(do_observer)) {

    }

    ~DoProducer() {
        DoDisposal();
    }

    void Run(const std::shared_ptr<ObservableCore>& source) {
        source_subscription_ = source->Subscribe(
            ObserverShim::FromWeak(As<ObserverCore>(shared_from_this())));
    }

    void OnNext(const std::any& value) override {
        try {
            do_observer_->OnNext(value);
        }
        catch (...) {
            // Exception thrown in the OnNext handler of the Do observer should be propagated to 
            // the downstream OnError handler.
            EmitOnError(std::current_exception());
            return;
        }
        EmitOnNext(value);
    }

    void OnError(const std::exception_ptr& error) override {
        try {
            do_observer_->OnError(error);
        }
        catch (...) {
            // Exception thrown in the OnError handler of the Do observer should be propagated to 
            // the downstream OnError handler.
            EmitOnError(std::current_exception());
            return;
        }
        EmitOnError(error);
    }

    void OnCompleted() override {
        try {
            do_observer_->OnCompleted();
        }
        catch (...) {
            // Exception thrown in the OnCompleted handler of the Do observer should be propagated 
            // to the downstream OnError handler.
            EmitOnError(std::current_exception());
            return;
        }
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
        do_observer_.reset();
    }

private:
    std::shared_ptr<Disposable> source_subscription_;
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


std::shared_ptr<Disposable> DoOperator::Subscribe(ObserverShim&& observer) {

    auto producer = std::make_shared<DoProducer>(std::move(observer), do_observer_);
    producer->Run(source_);
    return producer;
}

}