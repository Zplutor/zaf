#pragma once

#include <zaf/rx/internal/observable/inner_observable.h>
#include <zaf/rx/work.h>

namespace zaf::internal {

class DoOnTerminatedOperator : public InnerObservable {
public:
    DoOnTerminatedOperator(
        std::shared_ptr<InnerObservable> source,
        Work on_terminated);

    std::shared_ptr<InnerSubscription> Subscribe(
        const std::shared_ptr<InnerObserver>& observer) override;

private:
    std::shared_ptr<InnerObservable> source_;
    Work on_terminated_;
};

}