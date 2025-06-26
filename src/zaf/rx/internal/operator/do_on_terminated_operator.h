#pragma once

#include <zaf/rx/internal/observable/observable_core.h>
#include <zaf/rx/work.h>

namespace zaf::rx::internal {

class DoOnTerminatedOperator : public ObservableCore {
public:
    DoOnTerminatedOperator(
        std::shared_ptr<ObservableCore> source,
        Work on_terminated);

    std::shared_ptr<InnerSubscription> Subscribe(
        const std::shared_ptr<ObserverCore>& observer) override;

private:
    std::shared_ptr<ObservableCore> source_;
    Work on_terminated_;
};

}