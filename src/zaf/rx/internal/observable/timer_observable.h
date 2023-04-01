#pragma once

#include <chrono>
#include <optional>
#include <zaf/rx/internal/observable/inner_observable.h>

namespace zaf::internal {

class TimerObservable : public InnerObservable {
public:
    TimerObservable(
        std::chrono::steady_clock::duration delay,
        std::optional<std::chrono::steady_clock::duration> interval,
        std::shared_ptr<Scheduler> scheduler);

    std::shared_ptr<SubscriptionCore> Subscribe(
        const std::shared_ptr<InnerObserver>& observer) override;

private:
    std::chrono::steady_clock::duration delay_;
    std::optional<std::chrono::steady_clock::duration> interval_;
    std::shared_ptr<Scheduler> scheduler_;
};

}