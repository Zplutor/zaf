#pragma once

#include <zaf/rx/internal/observable/inner_observable.h>

namespace zaf::internal {

class DoOperator : public InnerObservable {
public:
    DoOperator(
        std::shared_ptr<InnerObservable> source,
        std::shared_ptr<InnerObserver> do_observer);

    std::shared_ptr<InnerSubscription> Subscribe(
        const std::shared_ptr<InnerObserver>& observer) override;

private:
    std::shared_ptr<InnerObservable> source_;
    std::shared_ptr<InnerObserver> do_observer_;
};

}