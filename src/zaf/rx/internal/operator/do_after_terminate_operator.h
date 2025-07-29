#pragma once

#include <zaf/rx/internal/observable/observable_core.h>

namespace zaf::rx::internal {

class DoAfterTerminateOperator : public ObservableCore {
public:
    DoAfterTerminateOperator(
        std::shared_ptr<ObservableCore> source,
        Closure after_terminate);

    std::shared_ptr<SubscriptionCore> Subscribe(
        const std::shared_ptr<ObserverCore>& observer) override;

private:
    std::shared_ptr<ObservableCore> source_;
    Closure after_terminate_;
};

}