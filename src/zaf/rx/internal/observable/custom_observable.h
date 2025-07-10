#pragma once

#include <zaf/rx/internal/observable/observable_core.h>

namespace zaf::rx::internal {

class CustomObservable : public ObservableCore {
public:
    using Procedure = std::function<
        void(std::shared_ptr<ObserverCore>, std::shared_ptr<SubscriptionCore>)
    >;

public:
    explicit CustomObservable(Procedure procedure);

    std::shared_ptr<SubscriptionCore> Subscribe(
        const std::shared_ptr<ObserverCore>& observer) override;

private:
    Procedure procedure_;
};


}