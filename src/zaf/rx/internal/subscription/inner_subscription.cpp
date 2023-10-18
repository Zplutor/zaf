#include <zaf/rx/internal/subscription/inner_subscription.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::internal {

const std::shared_ptr<InnerSubscription>& InnerSubscription::Empty() {
    
    static auto empty_subscription = std::make_shared<InnerSubscription>(nullptr);
    return empty_subscription;
}


InnerSubscription::InnerSubscription(std::shared_ptr<internal::Producer> producer) :
    producer_(std::move(producer)) {

    if (producer_) {
        producer_->RegisterFinishNotification([this](internal::Producer*, int) {
            producer_->Dispose();
        });
    }
}


InnerSubscription::~InnerSubscription() {
    Unsubscribe();
}


void InnerSubscription::Unsubscribe() {
    if (producer_) {
        producer_->Dispose();
    }
}

}