#include <zaf/rx/internal/operator/flat_mapper.h>
#include <zaf/base/as.h>
#include <zaf/base/container/utility/contain.h>
#include <zaf/base/container/utility/map.h>
#include <zaf/rx/internal/inner_observer.h>
#include <zaf/rx/internal/operator/flat_map_operator.h>
#include <zaf/rx/internal/producer.h>
#include <zaf/rx/internal/subscription/inner_subscription.h>

namespace zaf::internal {
namespace {

class FlatMapProducer : public Producer, public InnerObserver {
public:
    FlatMapProducer(
        std::shared_ptr<InnerObserver> next_observer,
        FlatMapper mapper)
        :
        Producer(std::move(next_observer)),
        mapper_(std::move(mapper)) {

    }

    void Run(const std::shared_ptr<InnerObservable>& source) {
        source_subscription_ = source->Subscribe(As<InnerObserver>(shared_from_this()));
    }

    void OnNext(const std::any& value) override {

        std::shared_ptr<InnerObservable> mapped_observable;
        try {
            mapped_observable = mapper_(value);
        }
        catch (...) {
            source_subscription_->Unsubscribe();
            TryToDeliverOnError(std::current_exception());
            return;
        }

        auto sub_id = ++mapper_subs_count_;
        auto mapper_sub = mapped_observable->Subscribe(InnerObserver::Create(
            [this](const std::any& value) {
                if (!IsTerminated()) {
                    EmitOnNext(value);
                }
            },
            [this, sub_id](const std::exception_ptr& error) {
                //Observer will be destroyed once the subscription is disposed, hence we keep the 
                //subscription alive until we have done everything.
                auto sub = OnMapperSubFinished(sub_id);
                TryToDeliverOnError(error);
            },
            [this, sub_id]() {
                //Same as above.
                auto sub = OnMapperSubFinished(sub_id);
                TryToDeliverOnCompleted();
            }
        ));

        if (!Contain(finished_mapper_subs_, sub_id)) {
            emitting_mapper_subs_[sub_id] = mapper_sub;
        }
    }

    void OnError(const std::exception_ptr& error) override {
        TryToDeliverOnError(error);
    }

    void OnCompleted() override {
        is_source_finished_ = true;
        TryToDeliverOnCompleted();
    }

    void OnDispose() override {

        source_subscription_->Unsubscribe();
        source_subscription_.reset();

        emitting_mapper_subs_.clear();
        mapper_ = nullptr;
    }

private:
    std::shared_ptr<InnerSubscription> OnMapperSubFinished(std::size_t sub_id) {

        std::shared_ptr<InnerSubscription> sub;
        auto iterator = emitting_mapper_subs_.find(sub_id);
        if (iterator != emitting_mapper_subs_.end()) {

            sub = iterator->second;
            emitting_mapper_subs_.erase(iterator);
        }
        
        finished_mapper_subs_.insert(sub_id);
        return sub;
    }

    void TryToDeliverOnError(const std::exception_ptr& error) {
        if (!IsTerminated()) {
            EmitOnError(error);
        }
    }

    void TryToDeliverOnCompleted() {

        if (IsTerminated()) {
            return;
        }

        if (is_source_finished_ && finished_mapper_subs_.size() == mapper_subs_count_) {
            EmitOnCompleted();
        }
    }

private:
    std::shared_ptr<InnerSubscription> source_subscription_;
    FlatMapper mapper_;
    std::size_t mapper_subs_count_{};

    bool is_source_finished_{};
    std::map<std::size_t, std::shared_ptr<InnerSubscription>> emitting_mapper_subs_;
    std::set<std::size_t> finished_mapper_subs_;
};

}

FlatMapOperator::FlatMapOperator(std::shared_ptr<InnerObservable> source, FlatMapper mapper) :
    source_(std::move(source)),
    mapper_(std::move(mapper)) {

}


std::shared_ptr<InnerSubscription> FlatMapOperator::Subscribe(
    const std::shared_ptr<InnerObserver>& observer) {

    auto producer = std::make_shared<FlatMapProducer>(observer, mapper_);
    producer->Run(source_);
    return std::make_shared<InnerSubscription>(std::move(producer));
}

}