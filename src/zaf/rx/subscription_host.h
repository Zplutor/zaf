#pragma once

#include <zaf/rx/subscription_holder.h>

namespace zaf {

class SubscriptionHost {
public:
    SubscriptionHost() = default;
    ~SubscriptionHost() = default;

    SubscriptionHost(const SubscriptionHost&) = delete;
    SubscriptionHost& operator=(const SubscriptionHost&) = delete;

    SubscriptionHolder& Subscriptions() {

        if (!subscription_holder_) {
            subscription_holder_ = std::make_unique<zaf::SubscriptionHolder>();
        }

        return *subscription_holder_;
    }

private:
    std::unique_ptr<zaf::SubscriptionHolder> subscription_holder_;
};

}