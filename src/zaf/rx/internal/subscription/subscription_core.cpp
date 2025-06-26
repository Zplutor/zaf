#include <zaf/rx/internal/subscription/subscription_core.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::rx::internal {

const std::shared_ptr<SubscriptionCore>& SubscriptionCore::Empty() {
    
    static auto empty_subscription = std::make_shared<SubscriptionCore>(nullptr);
    return empty_subscription;
}


SubscriptionCore::SubscriptionCore(std::shared_ptr<internal::Producer> producer) :
    producer_(std::move(producer)) {

    if (producer_) {
        producer_->RegisterTerminateNotification([this]() {
            producer_->Dispose();
        });
    }
}


SubscriptionCore::~SubscriptionCore() {
    Unsubscribe();
}


void SubscriptionCore::Unsubscribe() {
    if (producer_) {
        producer_->Dispose();
    }
}

}