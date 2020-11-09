#pragma once

#include <mutex>
#include <zaf/rx/internal/observable_implementation.h>
#include <zaf/rx/internal/observer_implementation.h>
#include <zaf/rx/internal/subscription/subscription_core.h>

namespace zaf::internal {

class SubjectCore : 
    public ObservableImplementation, 
    public ObserverImplementation, 
    public std::enable_shared_from_this<SubjectCore> {

public:
    ~SubjectCore();

    std::shared_ptr<SubscriptionImplementation> Subscribe(
        const std::shared_ptr<ObserverImplementation>& observer) override;

    void OnNext(const std::any& value) override;
    void OnError(const Error& error) override;
    void OnCompleted() override;

    void Unsubscribe(SubscriptionCore* subscription_core);

private:
    std::vector<std::shared_ptr<SubscriptionCore>> subscription_cores_;
};

}