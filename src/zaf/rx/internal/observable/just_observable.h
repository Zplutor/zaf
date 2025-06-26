#pragma once

#include <zaf/rx/internal/observable/observable_core.h>

namespace zaf::rx::internal {

class JustObservable : public ObservableCore {
public:
    JustObservable(std::any value);

    std::shared_ptr<InnerSubscription> Subscribe(
        const std::shared_ptr<ObserverCore>& observer) override;

private:
    std::any value_;
};

}