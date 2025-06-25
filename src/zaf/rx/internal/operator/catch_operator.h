#pragma once

#include <zaf/rx/internal/observable/observable_core.h>
#include <zaf/rx/internal/operator/catch_handler.h>

namespace zaf::internal {

class CatchOperator : public ObservableCore {
public:
    CatchOperator(
        std::shared_ptr<ObservableCore> source,
        CatchHandler handler);

    std::shared_ptr<InnerSubscription> Subscribe(
        const std::shared_ptr<ObserverCore>& observer) override;

private:
    std::shared_ptr<ObservableCore> source_;
    CatchHandler handler_;
};

}