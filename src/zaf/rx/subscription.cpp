#include <zaf/rx/subscription.h>
#include <zaf/rx/internal/subscription/subscription_core.h>

namespace zaf {

Subscription::Subscription(std::shared_ptr<rx::internal::SubscriptionCore> core) noexcept :
    core_(std::move(core)) {

}


Subscription::~Subscription() {

}


void Subscription::Unsubscribe() {
    if (core_) {
        core_->Unsubscribe();
    }
}


bool Subscription::IsUnsubscribed() const noexcept {
    if (core_) {
        return core_->IsUnsubscribed();
    }
    return true;
}

}