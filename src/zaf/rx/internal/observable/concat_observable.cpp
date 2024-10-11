#include <zaf/rx/internal/observable/concat_observable.h>
#include <zaf/base/as.h>
#include <zaf/rx/internal/inner_observer.h>
#include <zaf/rx/internal/producer.h>
#include <zaf/rx/internal/subscription/inner_subscription.h>

namespace zaf::internal {
namespace {

class ConcatProducer : public Producer, public InnerObserver {
public:
    ConcatProducer(
        std::shared_ptr<ConcatObservable> concat_observable,
        std::shared_ptr<InnerObserver> observer)
        :
        Producer(std::move(observer)),
        concat_observable_(std::move(concat_observable)) {

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
    void OnDispose() override {

        if (current_sub_) {
            current_sub_->Unsubscribe();
            current_sub_.reset();
        }

        concat_observable_.reset();
    }

private:
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

        const auto& observable = observables[*current_index_];
        current_sub_ = observable->Subscribe(As<InnerObserver>(shared_from_this()));
    }

private:
    //Refer to ConcatObservable directly to avoid additional memory allocation.
    std::shared_ptr<ConcatObservable> concat_observable_;
    std::optional<std::size_t> current_index_{};
    std::shared_ptr<InnerSubscription> current_sub_;
};

}

ConcatObservable::ConcatObservable(ObservableList observables) :
    observables_(std::move(observables)) {

}


std::shared_ptr<InnerSubscription> ConcatObservable::Subscribe(
    const std::shared_ptr<InnerObserver>& observer) {

    auto producer = std::make_shared<ConcatProducer>(
        As<ConcatObservable>(shared_from_this()),
        observer);

    producer->Run();
    return std::make_shared<InnerSubscription>(std::move(producer));
}

}