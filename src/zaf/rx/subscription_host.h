#pragma once

#include <zaf/rx/subscription_bag.h>

namespace zaf {

class SubscriptionHost {
public:
    SubscriptionHost() = default;
    ~SubscriptionHost() = default;

    SubscriptionHost(const SubscriptionHost&) = delete;
    SubscriptionHost& operator=(const SubscriptionHost&) = delete;

    SubscriptionBag& Subscriptions() {
        return subscriptions_;
    }

private:
    SubscriptionBag subscriptions_;
};

}