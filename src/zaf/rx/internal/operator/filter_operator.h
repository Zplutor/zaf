#pragma once

#include <zaf/rx/internal/observable/observable_core.h>

namespace zaf::rx::internal {

class FilterOperator : public ObservableCore {
public:
    FilterOperator(std::shared_ptr<ObservableCore> source, FilterPredicate predicate);

    std::shared_ptr<Disposable> Subscribe(ObserverShim&& observer) override;

private:
    std::shared_ptr<ObservableCore> source_;
    FilterPredicate predicate_;
};

}