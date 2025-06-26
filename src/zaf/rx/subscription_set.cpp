#include <zaf/rx/subscription_set.h>
#include <zaf/rx/internal/subscription/inner_subscription_set.h>

namespace zaf {

SubscriptionSet::SubscriptionSet() : 
    inner_(std::make_unique<rx::internal::InnerSubscriptionSet>()) {

}


SubscriptionSet::~SubscriptionSet() {

}


SubscriptionSet& SubscriptionSet::operator+=(const Subscription& subscription) {
    inner_->Add(subscription.Core());
    return *this;
}


SubscriptionSet& SubscriptionSet::operator+=(const TagItem& item) {
    inner_->Add(item.tag, item.subscription.Core());
    return *this;
}


SubscriptionSet& SubscriptionSet::operator-=(const std::string& tag) {
    inner_->Remove(tag);
    return *this;
}


void SubscriptionSet::Clear() {
    inner_->Clear();
}


std::size_t SubscriptionSet::Count() const {
    return inner_->Count();
}

}