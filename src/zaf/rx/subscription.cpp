#include <zaf/rx/subscription.h>
#include <zaf/rx/internal/subscription/inner_subscription.h>

namespace zaf {

Subscription::Subscription() : inner_(rx::internal::InnerSubscription::Empty()) {

}


Subscription::Subscription(Subscription&& other) : inner_(std::move(other.inner_)) {

    other.inner_ = rx::internal::InnerSubscription::Empty();
}


Subscription& Subscription::operator=(Subscription&& other) {

    inner_ = std::move(other.inner_);
    other.inner_ = rx::internal::InnerSubscription::Empty();
    return *this;
}


void Subscription::Unsubscribe() {
    inner_->Unsubscribe();
}

}