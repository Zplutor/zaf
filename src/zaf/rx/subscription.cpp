#include <zaf/rx/subscription.h>
#include <zaf/base/error/check.h>
#include <zaf/rx/internal/subscription/empty_subscription_core.h>

namespace zaf {

Subscription::Subscription() : core_(internal::EmptySubscriptionCore::Instance()) {

}


Subscription::Subscription(std::shared_ptr<internal::SubscriptionCore> core) : 
    core_(std::move(core)) {

    ZAF_EXPECT(core_);
}


Subscription::Subscription(Subscription&& other) : core_(std::move(other.core_)) {

    other.core_ = internal::EmptySubscriptionCore::Instance();
}


Subscription& Subscription::operator=(Subscription&& other) {

    core_ = std::move(other.core_);
    other.core_ = internal::EmptySubscriptionCore::Instance();
    return *this;
}


void Subscription::Unsubscribe() {
    core_->Unsubscribe();
}

}