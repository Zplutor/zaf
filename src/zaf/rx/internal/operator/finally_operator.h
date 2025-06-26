#pragma once

#include <zaf/rx/internal/observable/observable_core.h>
#include <zaf/rx/work.h>

namespace zaf::rx::internal {

class FinallyOperator : public ObservableCore {
public:
    FinallyOperator(
        std::shared_ptr<ObservableCore> source,
        Work finally_work);

    std::shared_ptr<SubscriptionCore> Subscribe(
        const std::shared_ptr<ObserverCore>& observer) override;

private:
    std::shared_ptr<ObservableCore> source_;
    Work finally_work_;
};

}