#include <zaf/rx/subscription.h>
#include <zaf/rx/internal/subscription/subscription_core.h>

namespace zaf {

Subscription::Subscription() : core_(rx::internal::SubscriptionCore::Empty()) {

}


Subscription::Subscription(Subscription&& other) : core_(std::move(other.core_)) {

    other.core_ = rx::internal::SubscriptionCore::Empty();
}


Subscription& Subscription::operator=(Subscription&& other) {

    core_ = std::move(other.core_);
    other.core_ = rx::internal::SubscriptionCore::Empty();
    return *this;
}


void Subscription::Unsubscribe() {
    core_->Unsubscribe();
}

}