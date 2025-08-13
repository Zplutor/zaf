#pragma once

#include <zaf/rx/internal/observable/observable_core.h>

namespace zaf::rx::internal {

class JustObservable : public ObservableCore {
public:
    JustObservable(std::any value);

    std::shared_ptr<SubscriptionCore> Subscribe(ObserverShim&& observer) override;

private:
    std::any value_;
};

}