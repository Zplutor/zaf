#include <zaf/rx/internal/subscription/inner_subscription.h>
#include <zaf/rx/internal/subscription/subscription_core.h>

namespace zaf::internal {

std::shared_ptr<InnerSubscription> InnerSubscription::Empty() {
    
    static auto empty_subscription = std::make_shared<InnerSubscription>(nullptr);
    return empty_subscription;
}


InnerSubscription::InnerSubscription(std::shared_ptr<SubscriptionCore> core) : core_(std::move(core)) {

}


InnerSubscription::~InnerSubscription() {
    Unsubscribe();
}


void InnerSubscription::Unsubscribe() {
    if (core_) {
        core_->Unsubscribe();
    }
}

}