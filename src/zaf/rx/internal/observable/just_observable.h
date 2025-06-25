#pragma once

#include <zaf/rx/internal/observable/observable_core.h>

namespace zaf::internal {

class JustObservable : public ObservableCore {
public:
    JustObservable(std::any value);

    std::shared_ptr<InnerSubscription> Subscribe(
        const std::shared_ptr<InnerObserver>& observer) override;

private:
    std::any value_;
};

}