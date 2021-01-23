#include <zaf/rx/subscription_holder.h>
#include <zaf/rx/internal/subscription/inner_subscription_holder.h>

namespace zaf {

SubscriptionHolder::SubscriptionHolder() : 
    inner_(std::make_unique<internal::InnerSubscriptionHolder>()) {

}


SubscriptionHolder::~SubscriptionHolder() {

}


SubscriptionHolder& SubscriptionHolder::operator+=(const Subscription& subscription) {
    inner_->Add(subscription.GetInner());
    return *this;
}


SubscriptionHolder& SubscriptionHolder::operator+=(const TagItem& item) {
    inner_->Add(item.tag, item.subscription.GetInner());
    return *this;
}


SubscriptionHolder& SubscriptionHolder::operator-=(const std::string& tag) {
    inner_->Remove(tag);
    return *this;
}


void SubscriptionHolder::Clear() {
    inner_->Clear();
}

}