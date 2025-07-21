#pragma once

#include <zaf/rx/internal/observable/observable_core.h>

namespace zaf::rx::internal {

class SubjectCore;

class ConnectableObservableCore : public ObservableCore {
public:
    ConnectableObservableCore(
        std::shared_ptr<ObservableCore> source,
        std::shared_ptr<SubjectCore> subject) noexcept;

    std::shared_ptr<SubscriptionCore> Subscribe(
        const std::shared_ptr<ObserverCore>& observer) override;

    std::shared_ptr<SubscriptionCore> Connect();

private:
    std::shared_ptr<ObservableCore> source_;
    std::shared_ptr<SubjectCore> subject_;
};

}
