#pragma once

#include <mutex>
#include <zaf/rx/internal/observable/observable_core.h>
#include <zaf/rx/internal/observer_core.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::rx::internal {

class SubjectCore : public ObservableCore, public ObserverCore {
public:
    ~SubjectCore();

    std::shared_ptr<SubscriptionCore> Subscribe(
        const std::shared_ptr<ObserverCore>& observer) override;

    void OnNext(const std::any& value) override;
    void OnError(const std::exception_ptr& error) override;
    void OnCompleted() override;

    //Called by SubjectProducer.
    void Unsubscribe(Producer* unsubscribed_producer);

    //For unit test.
    std::size_t SubscriptionCount() const {
        return producers_.size();
    }

private:
    std::vector<std::shared_ptr<Producer>> producers_;
};

}