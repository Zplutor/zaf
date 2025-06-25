#pragma once

#include <chrono>
#include <optional>
#include <zaf/rx/internal/observable/observable_core.h>

namespace zaf::internal {

class TimerObservable : public ObservableCore {
public:
    TimerObservable(
        std::chrono::steady_clock::duration delay,
        std::optional<std::chrono::steady_clock::duration> interval,
        std::shared_ptr<Scheduler> scheduler);

    std::shared_ptr<InnerSubscription> Subscribe(
        const std::shared_ptr<ObserverCore>& observer) override;

private:
    std::chrono::steady_clock::duration delay_;
    std::optional<std::chrono::steady_clock::duration> interval_;
    std::shared_ptr<Scheduler> scheduler_;
};

}