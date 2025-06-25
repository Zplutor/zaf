#pragma once

#include <zaf/rx/internal/observable/observable_core.h>

namespace zaf::internal {

class CustomizedObservable : public ObservableCore {
public:
    using Procedure = std::function<
        std::shared_ptr<InnerSubscription>(const std::shared_ptr<InnerObserver>&)
    >;

public:
    CustomizedObservable(Procedure procedure);

    std::shared_ptr<InnerSubscription> Subscribe(
        const std::shared_ptr<InnerObserver>& observer) override;

private:
    Procedure procedure_;
};


}