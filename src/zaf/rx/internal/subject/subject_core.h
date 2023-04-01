#pragma once

#include <mutex>
#include <zaf/rx/internal/observable/inner_observable.h>
#include <zaf/rx/internal/inner_observer.h>
#include <zaf/rx/internal/subscription/subscription_core.h>

namespace zaf::internal {

class SubjectCore : 
    public InnerObservable, 
    public InnerObserver {

public:
    ~SubjectCore();

    std::shared_ptr<SubscriptionCore> Subscribe(
        const std::shared_ptr<InnerObserver>& observer) override;

    void OnNext(const std::any& value) override;
    void OnError(const Error& error) override;
    void OnCompleted() override;

    //Called by SubjectSubscriptionCore.
    void Unsubscribe(SubscriptionCore* subscription_core);

private:
    std::vector<std::shared_ptr<SubscriptionCore>> subscription_cores_;
};

}