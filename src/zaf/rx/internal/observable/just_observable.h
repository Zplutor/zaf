#pragma once

#include <zaf/rx/internal/observable/inner_observable.h>

namespace zaf::internal {

class JustObservable : public InnerObservable {
public:
    JustObservable(std::any value);

    std::shared_ptr<InnerSubscription> Subscribe(
        const std::shared_ptr<InnerObserver>& observer) override;

private:
    std::any value_;
};

}