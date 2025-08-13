#pragma once

#include <zaf/rx/internal/observable/observable_core.h>

namespace zaf::rx::internal {

class NeverObservable : public ObservableCore {
public:
    static const std::shared_ptr<NeverObservable>& Instance();

public:
    std::shared_ptr<SubscriptionCore> Subscribe(ObserverShim&& observer) override;

private:
    NeverObservable() = default;
};

}