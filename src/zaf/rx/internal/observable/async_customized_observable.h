#pragma once

#include <zaf/rx/cancel.h>
#include <zaf/rx/internal/observable/inner_observable.h>

namespace zaf::internal {

class AsyncCustomizedObservable : public InnerObservable {
public:
    using AsyncProcedure = std::function<
        void(const std::shared_ptr<InnerObserver>&, CancelToken cancel_token)
    >;

public:
    AsyncCustomizedObservable(
        std::shared_ptr<Scheduler> scheduler,
        AsyncProcedure procedure);

    std::shared_ptr<SubscriptionCore> Subscribe(
        const std::shared_ptr<InnerObserver>& observer) override;

private:
    std::shared_ptr<Scheduler> scheduler_;
    AsyncProcedure procedure_;
};


}