#include <zaf/rx/internal/operator/flat_mapper.h>
#include <optional>
#include <zaf/base/as.h>
#include <zaf/base/container/utility/contain.h>
#include <zaf/base/container/utility/map.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/operator/flat_map_operator.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::rx::internal {
namespace {

class FlatMapProducer : public Producer, public ObserverCore {
public:
    FlatMapProducer(
        ObserverShim&& next_observer,
        FlatMapper mapper)
        :
        Producer(std::move(next_observer)),
        mapper_(std::move(mapper)) {

        emitting_mapper_subs_.emplace();
    }

    ~FlatMapProducer() {
        DoDisposal();
    }

    void Run(const std::shared_ptr<ObservableCore>& source) {
        source_sub_ = source->Subscribe(
            ObserverShim::FromWeak(As<ObserverCore>(shared_from_this())));
    }

    void OnNext(const std::any& value) override {

        std::shared_ptr<ObservableCore> mapped_observable;
        try {
            mapped_observable = mapper_(value);
        }
        catch (...) {
            EmitOnError(std::current_exception());
            return;
        }

        std::weak_ptr<FlatMapProducer> weak_this = As<FlatMapProducer>(shared_from_this());

        auto sub_id = ++mapper_subs_count_;
        auto mapper_sub = mapped_observable->Subscribe(ObserverShim::MakeShared(
            [weak_this](const std::any& value) {
                if (auto strong_this = weak_this.lock()) {
                    strong_this->EmitOnNext(value);
                }
            },
            [weak_this, sub_id](const std::exception_ptr& error) {
                if (auto strong_this = weak_this.lock()) {
                    strong_this->OnMapperTerminated(sub_id);
                    strong_this->EmitOnError(error);
                }
            },
            [weak_this, sub_id]() {
                if (auto strong_this = weak_this.lock()) {
                    strong_this->OnMapperTerminated(sub_id);
                    strong_this->TryEmitOnCompleted();
                }
            }
        ));
        
        if (mapper_sub) {
            bool should_dispose{};
            {
                std::lock_guard<std::mutex> lock(mutex_);
                if (emitting_mapper_subs_) {
                    if (!Contain(terminated_mapper_subs_, sub_id)) {
                        (*emitting_mapper_subs_)[sub_id] = std::move(mapper_sub);
                    }
                }
                else {
                    // If the producer is disposed, mapper subscription should be disposed 
                    // immediately.
                    should_dispose = true;
                }
            }
            if (should_dispose) {
                mapper_sub->Dispose();
            }
        }
    }

    void OnError(const std::exception_ptr& error) override {
        EmitOnError(error);
    }

    void OnCompleted() override {
        is_source_completed_ = true;
        TryEmitOnCompleted();
    }

    void OnDispose() noexcept override {
        DoDisposal();
    }

private:
    void OnMapperTerminated(std::size_t sub_id) {

        std::lock_guard<std::mutex> lock(mutex_);
        terminated_mapper_subs_.insert(sub_id);
        if (emitting_mapper_subs_) {
            emitting_mapper_subs_->erase(sub_id);
        }
    }

    void TryEmitOnCompleted() {

        bool is_totally_terminated{};
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (is_source_completed_ && (terminated_mapper_subs_.size() == mapper_subs_count_)) {
                is_totally_terminated = true;
            }
        }

        if (is_totally_terminated) {
            EmitOnCompleted();
        }
    }

    void DoDisposal() noexcept {

        if (source_sub_) {
            source_sub_->Dispose();
            source_sub_.reset();
        }

        std::map<std::size_t, std::shared_ptr<SubscriptionCore>> mapper_subs;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            mapper_subs = std::move(*emitting_mapper_subs_);
        }
        for (const auto& pair : mapper_subs) {
            pair.second->Dispose();
        }

        mapper_ = nullptr;
    }

private:
    std::shared_ptr<SubscriptionCore> source_sub_;
    FlatMapper mapper_;

    std::mutex mutex_;
    std::atomic<bool> is_source_completed_{};
    std::atomic<std::size_t> mapper_subs_count_{};
    // Will be std::nullopt if disposed.
    std::optional<std::map<std::size_t, std::shared_ptr<SubscriptionCore>>> emitting_mapper_subs_;
    std::set<std::size_t> terminated_mapper_subs_;
};

}

FlatMapOperator::FlatMapOperator(std::shared_ptr<ObservableCore> source, FlatMapper mapper) :
    source_(std::move(source)),
    mapper_(std::move(mapper)) {

}


std::shared_ptr<SubscriptionCore> FlatMapOperator::Subscribe(ObserverShim&& observer) {

    auto producer = std::make_shared<FlatMapProducer>(std::move(observer), mapper_);
    producer->Run(source_);
    return producer;
}

}