#include <zaf/rx/internal/operator/filter_operator.h>
#include <zaf/base/as.h>
#include <zaf/base/error/precondition_error.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::rx::internal {
namespace {

class FilterProducer : public Producer, public ObserverCore {
public:
    FilterProducer(ObserverShim&& observer, FilterPredicate predicate) noexcept :
        Producer(std::move(observer)),
        predicate_(std::move(predicate)) {

    }

    ~FilterProducer() {
        DoDisposal();
    }

    void Run(const std::shared_ptr<ObservableCore>& source) {
        source_sub_ = source->Subscribe(
            ObserverShim::FromWeak(As<ObserverCore>(shared_from_this())));
    }

    void OnNext(const std::any& value) override {

        bool can_emit{};
        try {
            can_emit = predicate_(value);
        }
        catch (...) {
            EmitOnError(std::current_exception());
            return;
        }

        if (can_emit) {
            EmitOnNext(value);
        }
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
        if (source_sub_) {
            source_sub_->Dispose();
            source_sub_.reset();
        }
    }

private:
    FilterPredicate predicate_;
    std::shared_ptr<Disposable> source_sub_;
};

}

FilterOperator::FilterOperator(std::shared_ptr<ObservableCore> source, FilterPredicate predicate) :
    source_(std::move(source)),
    predicate_(std::move(predicate)) {

    ZAF_EXPECT(source_);
    ZAF_EXPECT(predicate_);
}


std::shared_ptr<Disposable> FilterOperator::Subscribe(ObserverShim&& observer) {
    auto producer = std::make_shared<FilterProducer>(std::move(observer), predicate_);
    producer->Run(source_);
    return producer;
}

}