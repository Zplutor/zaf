#pragma once

#include <zaf/rx/internal/observable/observable_core.h>

namespace zaf::rx::internal {

using ObservableCoreList = std::vector<std::shared_ptr<ObservableCore>>;

class ConcatObservable : public ObservableCore {
public:
    explicit ConcatObservable(ObservableCoreList observables);

    const ObservableCoreList& Observables() const {
        return observables_;
    }

    std::shared_ptr<SubscriptionCore> Subscribe(
        const std::shared_ptr<ObserverCore>& observer) override;

private:
    ObservableCoreList observables_;
};

}