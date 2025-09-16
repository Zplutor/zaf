#pragma once

#include <zaf/rx/internal/observable/observable_core.h>

namespace zaf::rx::internal {

class SampleOperator : public ObservableCore {
public:
    SampleOperator(
        std::shared_ptr<ObservableCore> source,
        std::chrono::steady_clock::duration interval,
        std::shared_ptr<Scheduler> scheduler);

    std::shared_ptr<Disposable> Subscribe(ObserverShim&& observer) override;

private:
    std::shared_ptr<ObservableCore> source_;
    std::chrono::steady_clock::duration interval_;
    std::shared_ptr<Scheduler> scheduler_;
};

}