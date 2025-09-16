#pragma once

#include <zaf/rx/internal/observable/observable_core.h>

namespace zaf::rx::internal {

class ThrottleFirstOperator : public ObservableCore {
public:
    ThrottleFirstOperator(
        std::shared_ptr<ObservableCore>,
        std::chrono::steady_clock::duration duration) noexcept;

    std::shared_ptr<Disposable> Subscribe(ObserverShim&& observer) override;

private:
    std::shared_ptr<ObservableCore> source_;
    std::chrono::steady_clock::duration duration_{};
};

}