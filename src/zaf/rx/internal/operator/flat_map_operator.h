#pragma once

#include <zaf/rx/internal/observable/inner_observable.h>
#include <zaf/rx/internal/operator/flat_mapper.h>

namespace zaf::internal {

class FlatMapOperator : public InnerObservable {
public:
    FlatMapOperator(
        std::shared_ptr<InnerObservable> source,
        FlatMapper mapper);

    std::shared_ptr<InnerSubscription> Subscribe(
        const std::shared_ptr<InnerObserver>& observer) override;

private:
    std::shared_ptr<InnerObservable> source_;
    FlatMapper mapper_;
};

}