#include <zaf/rx/internal/operator/subscribe_on_operator.h>
#include <zaf/base/as.h>
#include <zaf/base/error/precondition_error.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::rx::internal {
namespace {

class SubscribeOnProducer : public Producer, public ObserverCore {
public:
    SubscribeOnProducer(
        std::shared_ptr<ObservableCore> source,
        std::shared_ptr<Scheduler> scheduler,
        ObserverShim&& observer) 
        :
        Producer(std::move(observer)),
        source_(std::move(source)),
        scheduler_(std::move(scheduler)) {
    
    }

    ~SubscribeOnProducer() {
        DoDisposal();
    }

    void Run() {

        scheduler_->ScheduleWork(std::bind(
            &SubscribeOnProducer::SubscribeOnScheduler, 
            As<SubscribeOnProducer>(shared_from_this())));
    }

    void OnDispose() noexcept override {
        DoDisposal();
    }

    void OnNext(const std::any& value) override {
        EmitOnNext(value);
    }

    void OnError(const std::exception_ptr& error) override {
        EmitOnError(error);
    }

    void OnCompleted() override {
        EmitOnCompleted();
    }

private:
    void SubscribeOnScheduler() {

        if (is_disposed_.load()) {
            return;
        }

        source_sub_ = source_->Subscribe(
            ObserverShim::FromWeak(As<SubscribeOnProducer>(shared_from_this())));
    }

    void DoDisposal() noexcept {

        bool expected{ false };
        if (!is_disposed_.compare_exchange_strong(expected, true)) {
            return;
        }

        scheduler_->ScheduleWork(
            [source = std::move(source_), source_sub = std::move(source_sub_)]() {
        
            if (source_sub) {
                source_sub->Dispose();
            }

            // Implicitly destroy the source and subscription in the scheduler thread.
        });
    }

private:
    std::shared_ptr<ObservableCore> source_;
    std::shared_ptr<Scheduler> scheduler_;
    std::shared_ptr<Disposable> source_sub_;
    std::atomic<bool> is_disposed_{};
};

}

SubscribeOnOperator::SubscribeOnOperator(
    std::shared_ptr<ObservableCore> source,
    std::shared_ptr<Scheduler> scheduler) 
    :
    source_(std::move(source)),
    scheduler_(std::move(scheduler)) {

    ZAF_EXPECT(source_);
    ZAF_EXPECT(scheduler_);
}


std::shared_ptr<Disposable> SubscribeOnOperator::Subscribe(ObserverShim&& observer) {

    auto producer = std::make_shared<SubscribeOnProducer>(
        source_, 
        scheduler_, 
        std::move(observer));

    producer->Run();
    return producer;
}

}