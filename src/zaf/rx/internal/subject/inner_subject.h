﻿#pragma once

#include <mutex>
#include <zaf/rx/internal/observable/inner_observable.h>
#include <zaf/rx/internal/inner_observer.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::internal {

class InnerSubject : 
    public InnerObservable, 
    public InnerObserver {

public:
    ~InnerSubject();

    std::shared_ptr<InnerSubscription> Subscribe(
        const std::shared_ptr<InnerObserver>& observer) override;

    void OnNext(const std::any& value) override;
    void OnError(const Error& error) override;
    void OnCompleted() override;

    //Called by SubjectSubscriptionCore.
    void Unsubscribe(Producer* subscription_core);

private:
    std::vector<std::shared_ptr<Producer>> subscription_cores_;
};

}