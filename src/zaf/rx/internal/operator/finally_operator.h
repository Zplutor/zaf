#pragma once

#include <zaf/rx/internal/observable/observable_core.h>

namespace zaf::rx::internal {

class FinallyOperator : public ObservableCore {
public:
    FinallyOperator(
        std::shared_ptr<ObservableCore> source,
        Closure finally_work);

    std::shared_ptr<Disposable> Subscribe(ObserverShim&& observer) override;

private:
    std::shared_ptr<ObservableCore> source_;
    Closure finally_work_;
};

}