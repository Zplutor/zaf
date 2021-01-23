#include <zaf/rx/subscription_host.h>

namespace zaf {

SubscriptionHolder& SubscriptionHost::Subscriptions() {

    std::call_once(subscription_holder_once_flag_, [this]() {
        subscription_holder_ = std::make_unique<zaf::SubscriptionHolder>();
    });

    return *subscription_holder_;
}

}