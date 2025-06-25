#pragma once

#include <zaf/rx/internal/observable/observable_core.h>

namespace zaf::internal {

class NeverObservable : public ObservableCore {
public:
    static const std::shared_ptr<NeverObservable>& Instance();

public:
    std::shared_ptr<InnerSubscription> Subscribe(
        const std::shared_ptr<InnerObserver>& observer) override;

private:
    NeverObservable() = default;
};

}