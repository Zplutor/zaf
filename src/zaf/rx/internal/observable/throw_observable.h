#pragma once

#include <zaf/rx/internal/observable/observable_core.h>
#include <exception>

namespace zaf::internal {

class ThrowObservable : public ObservableCore {
public:
    explicit ThrowObservable(std::exception_ptr error);

    std::shared_ptr<InnerSubscription> Subscribe(
        const std::shared_ptr<InnerObserver>& observer) override;

private:
    std::exception_ptr error_;
};

}