#pragma once

#include <chrono>
#include <zaf/rx/internal/observable/observable_core.h>
#include <zaf/rx/scheduler/scheduler.h>

namespace zaf::rx::internal {

class DebounceOperator : public ObservableCore {
public:
    DebounceOperator(
        std::shared_ptr<ObservableCore> source,
        std::chrono::steady_clock::duration duration,
        std::shared_ptr<Scheduler> scheduler);

    std::shared_ptr<Disposable> Subscribe(ObserverShim&& observer) override;

private:
    std::shared_ptr<ObservableCore> source_;
    std::chrono::steady_clock::duration duration_{};
    std::shared_ptr<Scheduler> scheduler_;
};

}