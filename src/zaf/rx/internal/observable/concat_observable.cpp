#include <zaf/rx/internal/observable/concat_observable.h>
#include <zaf/base/as.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::rx::internal {
namespace {

class ConcatProducer : public Producer, public ObserverCore {
public:
    ConcatProducer(
        std::shared_ptr<ConcatObservable> concat_observable,
        ObserverShim&& observer)
        :
        Producer(std::move(observer)),
        concat_observable_(std::move(concat_observable)) {

    }

    ~ConcatProducer() {
        DoDisposal();
    }

    void Run() {
        SubscribeToNextObservable();
    }


    void OnNext(const std::any& value) override {
        EmitOnNext(value);
    }


    void OnError(const std::exception_ptr& error) override {
        EmitOnError(error);
    }


    void OnCompleted() override {
        SubscribeToNextObservable();
    }

protected:
    void OnDispose() noexcept override {
        DoDisposal();
    }

private:
    //This method is re-entrant.
    void SubscribeToNextObservable() {

        if (!current_index_) {
            current_index_ = 0;
        }
        else {
            ++*current_index_;
        }

        const auto& observables = concat_observable_->Observables();
        if (*current_index_ >= observables.size()) {
            EmitOnCompleted();
            return;
        }

        std::size_t current_call_index = *current_index_;

        const auto& observable = observables[*current_index_];
        auto sub = observable->Subscribe(
            ObserverShim::FromWeak(As<ObserverCore>(shared_from_this())));

        //Don't override the subscription of re-entrant calls.
        if (current_call_index == *current_index_) {
            current_sub_ = std::move(sub);
        }
    }

    void DoDisposal() noexcept {

        if (current_sub_) {
            current_sub_->Dispose();
            current_sub_.reset();
        }

        concat_observable_.reset();
    }

private:
    //Refer to ConcatObservable directly to avoid additional memory allocation.
    std::shared_ptr<ConcatObservable> concat_observable_;
    std::optional<std::size_t> current_index_{};
    std::shared_ptr<SubscriptionCore> current_sub_;
};

}

ConcatObservable::ConcatObservable(ObservableCoreList observables) :
    observables_(std::move(observables)) {

}


std::shared_ptr<SubscriptionCore> ConcatObservable::Subscribe(ObserverShim&& observer) {

    auto producer = std::make_shared<ConcatProducer>(
        As<ConcatObservable>(shared_from_this()),
        std::move(observer));

    producer->Run();
    return producer;
}

}