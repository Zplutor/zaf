#include <zaf/rx/subscription.h>
#include <zaf/rx/internal/subscription/subscription_core.h>

namespace zaf {

Subscription::Subscription(std::shared_ptr<rx::internal::SubscriptionCore> core) : 
    core_(std::move(core)) {

}


void Subscription::Unsubscribe() {
    if (core_) {
        core_->Unsubscribe();
    }
}

}