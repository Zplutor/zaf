#pragma once

#include <zaf/rx/internal/observable/observable_core.h>
#include <zaf/rx/internal/operator/catch_handler.h>

namespace zaf::rx::internal {

class CatchOperator : public ObservableCore {
public:
    CatchOperator(
        std::shared_ptr<ObservableCore> source,
        CatchHandler handler);

    std::shared_ptr<Disposable> Subscribe(ObserverShim&& observer) override;

private:
    std::shared_ptr<ObservableCore> source_;
    CatchHandler handler_;
};

}