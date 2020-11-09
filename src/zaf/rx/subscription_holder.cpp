#include <zaf/rx/subscription_holder.h>
#include <zaf/rx/internal/subscription/subscription_holder_implementation.h>

namespace zaf {

SubscriptionHolder::SubscriptionHolder() : 
    implementation_(std::make_unique<internal::SubscriptionHolderImplementation>()) {

}


SubscriptionHolder::~SubscriptionHolder() {

}


SubscriptionHolder& SubscriptionHolder::operator+=(const Subscription& subscription) {
    implementation_->Add(subscription.GetImplementation());
    return *this;
}


SubscriptionHolder& SubscriptionHolder::operator+=(const TagItem& item) {
    implementation_->Add(item.tag, item.subscription.GetImplementation());
    return *this;
}


SubscriptionHolder& SubscriptionHolder::operator-=(const std::string& tag) {
    implementation_->Remove(tag);
    return *this;
}


void SubscriptionHolder::Clear() {
    implementation_->Clear();
}

}