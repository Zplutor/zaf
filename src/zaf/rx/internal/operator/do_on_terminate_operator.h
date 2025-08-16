#pragma once

#include <zaf/rx/internal/observable/observable_core.h>

namespace zaf::rx::internal {

class DoOnTerminateOperator : public ObservableCore {
public:
    DoOnTerminateOperator(
        std::shared_ptr<ObservableCore> source,
        Closure on_terminate);

    std::shared_ptr<Disposable> Subscribe(ObserverShim&& observer) override;

private:
    std::shared_ptr<ObservableCore> source_;
    Closure on_terminate_;
};

}