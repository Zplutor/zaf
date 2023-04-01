#pragma once

#include <zaf/rx/internal/observable/inner_observable.h>
#include <zaf/rx/scheduler.h>

namespace zaf::internal {

class ObserveOnOperator : public InnerObservable {
public:
    ObserveOnOperator(
        std::shared_ptr<InnerObservable> source,
        std::shared_ptr<Scheduler> scheduler);

    std::shared_ptr<SubscriptionCore> Subscribe(
        const std::shared_ptr<InnerObserver>& observer) override;

private:
    std::shared_ptr<InnerObservable> source_;
    std::shared_ptr<Scheduler> scheduler_;
};

}