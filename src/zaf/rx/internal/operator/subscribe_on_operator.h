#pragma once

#include <zaf/rx/internal/observable/observable_core.h>
#include <zaf/rx/scheduler.h>

namespace zaf::internal {

class SubscribeOnOperator : public ObservableCore {
public:
    SubscribeOnOperator(
        std::shared_ptr<ObservableCore> source,
        std::shared_ptr<Scheduler> scheduler);

    std::shared_ptr<InnerSubscription> Subscribe(
        const std::shared_ptr<InnerObserver>& observer) override;

private:
    std::shared_ptr<ObservableCore> source_;
    std::shared_ptr<Scheduler> scheduler_;
};

}