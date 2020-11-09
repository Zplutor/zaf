#include <zaf/rx/subscription.h>
#include <zaf/rx/internal/subscription/subscription_implementation.h>

namespace zaf {

Subscription::Subscription() : implementation_(internal::SubscriptionImplementation::Empty()) {

}


Subscription::Subscription(Subscription&& other) : 
    implementation_(std::move(other.implementation_)) {

    other.implementation_ = internal::SubscriptionImplementation::Empty();
}


Subscription& Subscription::operator=(Subscription&& other) {

    implementation_ = std::move(other.implementation_);
    other.implementation_ = internal::SubscriptionImplementation::Empty();
    return *this;
}


void Subscription::Unsubscribe() {
    implementation_->Unsubscribe();
}

}