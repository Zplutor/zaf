#pragma once

#include <zaf/rx/internal/observable/observable_core.h>
#include <zaf/rx/internal/operator/flat_mapper.h>

namespace zaf::internal {

class FlatMapOperator : public ObservableCore {
public:
    FlatMapOperator(
        std::shared_ptr<ObservableCore> source,
        FlatMapper mapper);

    std::shared_ptr<InnerSubscription> Subscribe(
        const std::shared_ptr<InnerObserver>& observer) override;

private:
    std::shared_ptr<ObservableCore> source_;
    FlatMapper mapper_;
};

}