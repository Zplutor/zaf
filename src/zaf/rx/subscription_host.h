#pragma once

#include <mutex>
#include <zaf/rx/subscription_holder.h>

namespace zaf {

class SubscriptionHost {
public:
    SubscriptionHost() = default;
    ~SubscriptionHost() = default;

    SubscriptionHost(const SubscriptionHost&) = delete;
    SubscriptionHost& operator=(const SubscriptionHost&) = delete;

    SubscriptionHolder& Subscriptions();

private:
    std::unique_ptr<zaf::SubscriptionHolder> subscription_holder_;
    std::once_flag subscription_holder_once_flag_;
};

}