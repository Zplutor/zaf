#pragma once

#include <zaf/rx/internal/observable/observable_core.h>

namespace zaf::rx::internal {

class SubjectCore;

class ConnectableObservableCore : public ObservableCore {
public:
    ConnectableObservableCore(
        std::shared_ptr<ObservableCore> source,
        std::shared_ptr<SubjectCore> subject) noexcept;

    std::shared_ptr<SubscriptionCore> Subscribe(ObserverShim&& observer) override;

    std::shared_ptr<SubscriptionCore> Connect();
    std::shared_ptr<ObservableCore> RefCount();

private:
    std::shared_ptr<ObservableCore> source_;
    std::shared_ptr<SubjectCore> subject_;
};

}
