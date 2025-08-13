#include <zaf/rx/internal/subject/multicast_observer.h>
#include <zaf/base/as.h>
#include <zaf/base/container/utility/erase.h>
#include <zaf/rx/internal/subscription/producer_subscription_core.h>

namespace zaf::rx::internal {

class MulticastObserver::IndividualProducer : public Producer {
public:
    IndividualProducer(
        ObserverShim&& observer,
        std::weak_ptr<MulticastObserver> owner)
        : 
        Producer(std::move(observer)),
        owner_(std::move(owner)) {

    }

private:
    void OnDispose() noexcept override {
        auto owner = owner_.lock();
        if (owner) {
            owner->RemoveProducer(this);
        }
    }

private:
    std::weak_ptr<MulticastObserver> owner_;
};


MulticastObserver::~MulticastObserver() {

    //Dispose all producers.
    for (const auto& each_producer : producers_) {
        each_producer->Dispose();
    }
}


std::shared_ptr<SubscriptionCore> MulticastObserver::AddObserver(ObserverShim&& observer) {

    std::variant<std::monostate, std::exception_ptr, None> termination;
    {
        std::lock_guard<std::mutex> lock(lock_);
        if (std::holds_alternative<std::monostate>(termination_)) {

            auto producer = std::make_shared<IndividualProducer>(
                std::move(observer), 
                shared_from_this());

            producers_.push_back(producer);
            return std::make_shared<ProducerSubscriptionCore>(producer);
        }
        termination = termination_;
    }

    // The multicast observer is terminated, notify the observer immediately.
    auto exception = std::get_if<std::exception_ptr>(&termination);
    if (exception) {
        observer.OnError(*exception);
    }
    else if (std::holds_alternative<None>(termination)) {
        observer.OnCompleted();
    }
    return nullptr;
}


std::size_t MulticastObserver::ObserverCount() const noexcept {
    std::lock_guard<std::mutex> lock(lock_);
    return producers_.size();
}


void MulticastObserver::OnNext(const std::any& value) {
    std::vector<std::shared_ptr<IndividualProducer>> copied_producers;
    {
        std::lock_guard<std::mutex> lock(lock_);
        copied_producers = producers_;
    }
    for (const auto& each_producer : copied_producers) {
        each_producer->EmitOnNext(value);
    }
}


void MulticastObserver::OnError(const std::exception_ptr& error) {
    std::vector<std::shared_ptr<IndividualProducer>> copied_producers;
    {
        std::lock_guard<std::mutex> lock(lock_);
        copied_producers = producers_;
        termination_ = error;
    }
    for (const auto& each_producer : copied_producers) {
        each_producer->EmitOnError(error);
    }
}


void MulticastObserver::OnCompleted() {
    std::vector<std::shared_ptr<IndividualProducer>> copied_producers;
    {
        std::lock_guard<std::mutex> lock(lock_);
        copied_producers = producers_;
        termination_ = None{};
    }
    for (const auto& each_producer : copied_producers) {
        each_producer->EmitOnCompleted();
    }
}


void MulticastObserver::RemoveProducer(IndividualProducer* producer) {
    std::lock_guard<std::mutex> lock(lock_);
    EraseIf(producers_, [producer](const auto& each) {
        return each.get() == producer;
    });
}

}