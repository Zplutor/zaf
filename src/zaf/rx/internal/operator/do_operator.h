#pragma once

#include <zaf/rx/internal/observable/observable_core.h>

namespace zaf::internal {

class DoOperator : public ObservableCore {
public:
    DoOperator(
        std::shared_ptr<ObservableCore> source,
        std::shared_ptr<InnerObserver> do_observer);

    std::shared_ptr<InnerSubscription> Subscribe(
        const std::shared_ptr<InnerObserver>& observer) override;

private:
    std::shared_ptr<ObservableCore> source_;
    std::shared_ptr<InnerObserver> do_observer_;
};

}