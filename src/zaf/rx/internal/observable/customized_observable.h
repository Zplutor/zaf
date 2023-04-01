#pragma once

#include <zaf/rx/internal/observable/inner_observable.h>

namespace zaf::internal {

class CustomizedObservable : public InnerObservable {
public:
    using Procedure = std::function<
        std::shared_ptr<SubscriptionCore>(const std::shared_ptr<InnerObserver>&)
    >;

public:
    CustomizedObservable(Procedure procedure);

    std::shared_ptr<SubscriptionCore> Subscribe(
        const std::shared_ptr<InnerObserver>& observer) override;

private:
    Procedure procedure_;
};


}