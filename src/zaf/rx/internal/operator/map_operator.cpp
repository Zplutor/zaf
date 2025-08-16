#include <zaf/rx/internal/operator/map_operator.h>
#include <zaf/base/as.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::rx::internal {
namespace {

class MapProducer : public Producer, public ObserverCore {
public:
    MapProducer(ObserverShim&& next_observer, Mapper mapper) : 
        Producer(std::move(next_observer)),
        mapper_(std::move(mapper)) {

    }

    ~MapProducer() {
        DoDisposal();
    }

    void Run(const std::shared_ptr<ObservableCore>& source) {
        source_subscription_ = source->Subscribe(
            ObserverShim::FromWeak(As<ObserverCore>(shared_from_this())));
    }

    void OnNext(const std::any& value) override {

        std::any mapped_value;
        try {
            mapped_value = mapper_(value);
        }
        catch (...) {
            // Exception thrown in the mapper should be propagated to the downstream OnError 
            // handler.
            EmitOnError(std::current_exception());
            return;
        }
        EmitOnNext(mapped_value);
    }

    void OnError(const std::exception_ptr& error) override {
        EmitOnError(error);
    }

    void OnCompleted() override {
        EmitOnCompleted();
    }

protected:
    void OnDispose() noexcept override {
        DoDisposal();
    }

private:
    void DoDisposal() noexcept {
        if (source_subscription_) {
            source_subscription_->Dispose();
            source_subscription_.reset();
        }
        mapper_ = nullptr;
    }

private:
    std::shared_ptr<Disposable> source_subscription_;
    Mapper mapper_;
};

}

MapOperator::MapOperator(std::shared_ptr<ObservableCore> source, Mapper mapper) :
    source_(std::move(source)),
    mapper_(std::move(mapper)) {

}


std::shared_ptr<Disposable> MapOperator::Subscribe(ObserverShim&& observer) {

    auto producer = std::make_shared<MapProducer>(std::move(observer), mapper_);
    producer->Run(source_);
    return producer;
}

}