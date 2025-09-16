#include <zaf/rx/internal/operator/throttle_first_operator.h>
#include <zaf/base/as.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::rx::internal {
namespace {

class ThrottleFirstProducer : public Producer, public ObserverCore {
public:
    ThrottleFirstProducer(
        ObserverShim&& observer,
        std::chrono::steady_clock::duration duration)
        :
        Producer(std::move(observer)),
        duration_(duration) {

    }

    ~ThrottleFirstProducer() {
        DoDisposal();
    }

    void Run(const std::shared_ptr<ObservableCore>& source) {
        source_sub_ = source->Subscribe(
            ObserverShim::FromShared(As<ObserverCore>(shared_from_this())));
    }

    void OnNext(const std::any& value) override {
        auto now = std::chrono::steady_clock::now();
        {
            std::lock_guard lock(mutex_);
            if (now - last_emission_time_ < duration_) {
                return;
            }
            last_emission_time_ = now;
        }
        EmitOnNext(value);
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
            source_sub_ = nullptr;
        }
    }

private:
    std::chrono::steady_clock::duration duration_{};
    std::shared_ptr<Disposable> source_sub_;
    std::mutex mutex_;
    std::chrono::steady_clock::time_point last_emission_time_{};
};

} // namespace

ThrottleFirstOperator::ThrottleFirstOperator(
    std::shared_ptr<ObservableCore> source,
    std::chrono::steady_clock::duration duration) noexcept
    :
    source_(std::move(source)),
    duration_(duration) {

}


std::shared_ptr<Disposable> ThrottleFirstOperator::Subscribe(ObserverShim&& observer) {
    auto producer = std::make_shared<ThrottleFirstProducer>(std::move(observer), duration_);
    producer->Run(source_);
    return producer;
}

}