#pragma once

#include <zaf/rx/internal/observable/observable_core.h>

namespace zaf::rx::internal {

class EmptyObservable : public ObservableCore {
public:
    static const std::shared_ptr<EmptyObservable>& Instance();

public:
    std::shared_ptr<SubscriptionCore> Subscribe(ObserverShim&& observer) override;

private:
    EmptyObservable() = default;
};

}