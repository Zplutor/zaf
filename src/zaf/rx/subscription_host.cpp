#include <zaf/rx/subscription_host.h>

namespace zaf {

SubscriptionSet& SubscriptionHost::Subscriptions() {

    std::call_once(subscriptions_once_flag_, [this]() {
        subscriptions_ = std::make_unique<zaf::SubscriptionSet>();
    });

    return *subscriptions_;
}

}