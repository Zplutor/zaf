#pragma once

#include <zaf/rx/cancel.h>
#include <zaf/rx/internal/observable/observable_core.h>

namespace zaf::rx::internal {

class AsyncCustomizedObservable : public ObservableCore {
public:
    using AsyncProcedure = std::function<
        void(const std::shared_ptr<ObserverCore>&, CancelToken cancel_token)
    >;

public:
    AsyncCustomizedObservable(
        std::shared_ptr<Scheduler> scheduler,
        AsyncProcedure procedure);

    std::shared_ptr<SubscriptionCore> Subscribe(
        const std::shared_ptr<ObserverCore>& observer) override;

private:
    std::shared_ptr<Scheduler> scheduler_;
    AsyncProcedure procedure_;
};


}