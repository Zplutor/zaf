#pragma once

#include <zaf/rx/internal/observable/inner_observable.h>
#include <zaf/rx/internal/operator/catch_handler.h>

namespace zaf::internal {

class CatchOperator : public InnerObservable {
public:
    CatchOperator(
        std::shared_ptr<InnerObservable> source,
        CatchHandler handler);

    std::shared_ptr<InnerSubscription> Subscribe(
        const std::shared_ptr<InnerObserver>& observer) override;

private:
    std::shared_ptr<InnerObservable> source_;
    CatchHandler handler_;
};

}