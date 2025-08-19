#pragma once

#include <zaf/rx/internal/observable/observable_core.h>
#include <zaf/rx/schedulers/scheduler.h>

namespace zaf::rx::internal {

class ObserveOnOperator : public ObservableCore {
public:
    ObserveOnOperator(
        std::shared_ptr<ObservableCore> source,
        std::shared_ptr<Scheduler> scheduler);

    std::shared_ptr<Disposable> Subscribe(ObserverShim&& observer) override;

private:
    std::shared_ptr<ObservableCore> source_;
    std::shared_ptr<Scheduler> scheduler_;
};

}