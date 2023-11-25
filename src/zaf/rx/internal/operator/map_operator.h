#pragma once

#include <zaf/rx/internal/observable/inner_observable.h>
#include <zaf/rx/internal/operator/mapper.h>

namespace zaf::internal {

class MapOperator : public InnerObservable {
public:
    MapOperator(
        std::shared_ptr<InnerObservable> source,
        Mapper mapper);

    std::shared_ptr<InnerSubscription> Subscribe(
        const std::shared_ptr<InnerObserver>& observer) override;

private:
    std::shared_ptr<InnerObservable> source_;
    Mapper mapper_;
};

}