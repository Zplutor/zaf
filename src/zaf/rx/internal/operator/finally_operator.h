#pragma once

#include <zaf/rx/internal/observable/inner_observable.h>
#include <zaf/rx/work.h>

namespace zaf::internal {

class FinallyOperator : public InnerObservable {
public:
    FinallyOperator(
        std::shared_ptr<InnerObservable> source,
        Work finally_work);

    std::shared_ptr<InnerSubscription> Subscribe(
        const std::shared_ptr<InnerObserver>& observer) override;

private:
    std::shared_ptr<InnerObservable> source_;
    Work finally_work_;
};

}