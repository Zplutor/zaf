#include <zaf/rx/internal/subject/multicast_observer.h>
#include <zaf/base/as.h>
#include <zaf/base/container/utility/erase.h>
#include <zaf/rx/disposable.h>

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

    ~IndividualProducer() {
        DoDisposal();
    }

private:
    void OnDispose() noexcept override {
        DoDisposal();
    }

    void DoDisposal() noexcept {
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
        auto producer = each_producer.lock();
        if (producer) {
            producer->Dispose();
        }
    }
}


std::shared_ptr<Disposable> MulticastObserver::AddObserver(ObserverShim&& observer) {

    std::variant<std::monostate, std::exception_ptr, None> termination;
    {
        std::lock_guard<std::mutex> lock(lock_);
        if (std::holds_alternative<std::monostate>(termination_)) {

            auto producer = std::make_shared<IndividualProducer>(
                std::move(observer), 
                shared_from_this());

            producers_.push_back(producer);
            return producer;
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
    return Disposable::Empty();
}


std::size_t MulticastObserver::ObserverCount() const noexcept {
    std::lock_guard<std::mutex> lock(lock_);
    return producers_.size();
}


void MulticastObserver::OnNext(const std::any& value) {
    std::vector<std::weak_ptr<IndividualProducer>> copied_producers;
    {
        std::lock_guard<std::mutex> lock(lock_);
        copied_producers = producers_;
    }
    for (const auto& each_producer : copied_producers) {
        if (auto producer = each_producer.lock()) {
            producer->EmitOnNext(value);
        }
    }
}


void MulticastObserver::OnError(const std::exception_ptr& error) {
    std::vector<std::weak_ptr<IndividualProducer>> copied_producers;
    {
        std::lock_guard<std::mutex> lock(lock_);
        copied_producers = producers_;
        termination_ = error;
    }
    for (const auto& each_producer : copied_producers) {
        if (auto producer = each_producer.lock()) {
            producer->EmitOnError(error);
        }
    }
}


void MulticastObserver::OnCompleted() {
    std::vector<std::weak_ptr<IndividualProducer>> copied_producers;
    {
        std::lock_guard<std::mutex> lock(lock_);
        copied_producers = producers_;
        termination_ = None{};
    }
    for (const auto& each_producer : copied_producers) {
        if (auto producer = each_producer.lock()) {
            producer->EmitOnCompleted();
        }
    }
}


void MulticastObserver::RemoveProducer(IndividualProducer* producer) noexcept {
    std::lock_guard<std::mutex> lock(lock_);
    EraseIf(producers_, [producer](const auto& each) {
        auto each_shared = each.lock();
        return !each_shared || each_shared.get() == producer;
    });
}

}