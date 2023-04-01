#pragma once

#include <mutex>
#include <zaf/rx/subscription_set.h>

namespace zaf {

class SubscriptionHost {
public:
    SubscriptionHost() = default;
    ~SubscriptionHost() = default;

    SubscriptionHost(const SubscriptionHost&) = delete;
    SubscriptionHost& operator=(const SubscriptionHost&) = delete;

    SubscriptionSet& Subscriptions();

private:
    std::unique_ptr<zaf::SubscriptionSet> subscriptions_;
    std::once_flag subscriptions_once_flag_;
};

}