#include <zaf/rx/internal/operator/map_operator.h>
#include <zaf/base/as.h>
#include <zaf/rx/internal/producer.h>
#include <zaf/rx/internal/subscription/subscription_core.h>

namespace zaf::rx::internal {
namespace {

class MapProducer : public Producer, public ObserverCore {
public:
    MapProducer(std::shared_ptr<ObserverCore> next_observer, Mapper mapper) : 
        Producer(std::move(next_observer)),
        mapper_(std::move(mapper)) {

    }

    void Run(const std::shared_ptr<ObservableCore>& source) {
        source_subscription_ = source->Subscribe(As<ObserverCore>(shared_from_this()));
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
    std::shared_ptr<SubscriptionCore> source_subscription_;
    Mapper mapper_;
};

}

MapOperator::MapOperator(std::shared_ptr<ObservableCore> source, Mapper mapper) :
    source_(std::move(source)),
    mapper_(std::move(mapper)) {

}


std::shared_ptr<SubscriptionCore> MapOperator::Subscribe(
    const std::shared_ptr<ObserverCore>& observer) {

    auto producer = std::make_shared<MapProducer>(observer, mapper_);
    producer->Run(source_);
    return std::make_shared<SubscriptionCore>(std::move(producer));
}

}