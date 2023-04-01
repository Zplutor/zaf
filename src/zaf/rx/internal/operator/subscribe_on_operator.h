#pragma once

#include <zaf/rx/internal/observable/inner_observable.h>
#include <zaf/rx/scheduler.h>

namespace zaf::internal {

class SubscribeOnOperator : public InnerObservable {
public:
    SubscribeOnOperator(
        std::shared_ptr<InnerObservable> source,
        std::shared_ptr<Scheduler> scheduler);

    std::shared_ptr<SubscriptionCore> Subscribe(
        const std::shared_ptr<InnerObserver>& observer) override;

private:
    std::shared_ptr<InnerObservable> source_;
    std::shared_ptr<Scheduler> scheduler_;
};

}