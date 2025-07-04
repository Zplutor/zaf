#include <zaf/rx/internal/subscription/subscription_core.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::rx::internal {

SubscriptionCore::SubscriptionCore(std::shared_ptr<internal::Producer> producer) :
    producer_(std::move(producer)) {

}


SubscriptionCore::~SubscriptionCore() {
    Unsubscribe();
}


void SubscriptionCore::Unsubscribe() {
    if (producer_) {
        producer_->Dispose();
        producer_.reset();
    }
}

}