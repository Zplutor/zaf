#pragma once

#include <zaf/rx/internal/subscription/subscription_core.h>

namespace zaf::rx::internal {

class Producer;

/**
A middle layer of `SubscriptionCore` that delegates the calls to a `Producer`, aiming to avoid
circular reference between `Producer` and its observers.

@details
    In theory,`Producer` can directly implement `SubscriptionCore`, but it will cause circular 
    reference between `Producer` and its observers, which may hold a subscription returned from the
    producer, and the subscription is the producer itself. This will cause memory leak.
*/
class ProducerSubscriptionCore : public SubscriptionCore {
public:
    explicit ProducerSubscriptionCore(std::shared_ptr<internal::Producer> producer) noexcept;
    ~ProducerSubscriptionCore();

    void Dispose() noexcept override;
    bool IsDisposed() const noexcept override;

    std::optional<DisposeNotificationID> RegisterDisposeNotification(
        DisposeNotification callback) override;

    void UnregisterDisposeNotification(DisposeNotificationID id) override;

    const std::shared_ptr<internal::Producer>& Producer() const noexcept {
        return producer_;
    }

private:
    std::shared_ptr<internal::Producer> producer_;
};

}