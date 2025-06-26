#pragma once

#include <zaf/rx/internal/observable/observable_core.h>
#include <exception>

namespace zaf::rx::internal {

class ThrowObservable : public ObservableCore {
public:
    explicit ThrowObservable(std::exception_ptr error);

    std::shared_ptr<SubscriptionCore> Subscribe(
        const std::shared_ptr<ObserverCore>& observer) override;

private:
    std::exception_ptr error_;
};

}