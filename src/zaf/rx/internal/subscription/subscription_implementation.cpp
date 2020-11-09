#include <zaf/rx/internal/subscription/subscription_implementation.h>
#include <zaf/rx/internal/subscription/subscription_core.h>

namespace zaf::internal {

std::shared_ptr<SubscriptionImplementation> SubscriptionImplementation::Empty() {
    
    static auto empty_subscription = std::make_shared<SubscriptionImplementation>(nullptr);
    return empty_subscription;
}


SubscriptionImplementation::SubscriptionImplementation(std::shared_ptr<SubscriptionCore> core) : core_(std::move(core)) {

}


SubscriptionImplementation::~SubscriptionImplementation() {
    Unsubscribe();
}


void SubscriptionImplementation::Unsubscribe() {
    if (core_) {
        core_->Unsubscribe();
    }
}

}