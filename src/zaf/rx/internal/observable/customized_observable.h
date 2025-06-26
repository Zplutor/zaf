#pragma once

#include <zaf/rx/internal/observable/observable_core.h>

namespace zaf::rx::internal {

class CustomizedObservable : public ObservableCore {
public:
    using Procedure = std::function<
        std::shared_ptr<InnerSubscription>(const std::shared_ptr<ObserverCore>&)
    >;

public:
    CustomizedObservable(Procedure procedure);

    std::shared_ptr<InnerSubscription> Subscribe(
        const std::shared_ptr<ObserverCore>& observer) override;

private:
    Procedure procedure_;
};


}