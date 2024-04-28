#include <zaf/rx/internal/operator/map_operator.h>
#include <zaf/base/as.h>
#include <zaf/rx/internal/producer.h>
#include <zaf/rx/internal/subscription/inner_subscription.h>

namespace zaf::internal {
namespace {

class MapProducer : public Producer, public InnerObserver {
public:
    MapProducer(std::shared_ptr<InnerObserver> next_observer, Mapper mapper) : 
        Producer(std::move(next_observer)),
        mapper_(std::move(mapper)) {

    }

    void Run(const std::shared_ptr<InnerObservable>& source) {
        source_subscription_ = source->Subscribe(As<InnerObserver>(shared_from_this()));
    }

    void OnNext(const std::any& value) override {

        std::any mapped_value;
        try {
            mapped_value = mapper_(value);
        }
        catch (...) {
            source_subscription_->Unsubscribe();
            TryToDeliverOnError(std::current_exception());
            return;
        }

        EmitOnNext(mapped_value);
    }

    void OnError(const std::exception_ptr& error) override {
        TryToDeliverOnError(error);
    }

    void OnCompleted() override {
        if (!IsTerminated()) {
            EmitOnCompleted();
        }
    }

protected:
    void OnDispose() override {

        source_subscription_->Unsubscribe();
        source_subscription_.reset();

        mapper_ = nullptr;
    }

private:
    void TryToDeliverOnError(const std::exception_ptr& error) {
        if (!IsTerminated()) {
            EmitOnError(error);
        }
    }

private:
    std::shared_ptr<InnerSubscription> source_subscription_;
    Mapper mapper_;
};

}

MapOperator::MapOperator(std::shared_ptr<InnerObservable> source, Mapper mapper) :
    source_(std::move(source)),
    mapper_(std::move(mapper)) {

}


std::shared_ptr<InnerSubscription> MapOperator::Subscribe(
    const std::shared_ptr<InnerObserver>& observer) {

    auto producer = std::make_shared<MapProducer>(observer, mapper_);
    producer->Run(source_);
    return std::make_shared<InnerSubscription>(std::move(producer));
}

}