#include <zaf/rx/internal/subscription/producer_subscription_core.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::rx::internal {

ProducerSubscriptionCore::ProducerSubscriptionCore(
    std::shared_ptr<internal::Producer> producer) noexcept
    :
    producer_(std::move(producer)) {

}


ProducerSubscriptionCore::~ProducerSubscriptionCore() {
    producer_->Unsubscribe();
}


void ProducerSubscriptionCore::Unsubscribe() {
    producer_->Unsubscribe();
}


bool ProducerSubscriptionCore::IsUnsubscribed() const noexcept {
    return producer_->IsUnsubscribed();
}


std::optional<UnsubscribeNotificationID> ProducerSubscriptionCore::RegisterUnsubscribeNotification(
    UnsubscribeNotification callback) {

    return producer_->RegisterUnsubscribeNotification(std::move(callback));
}


void ProducerSubscriptionCore::UnregisterUnsubscribeNotification(UnsubscribeNotificationID id) {
    producer_->UnregisterUnsubscribeNotification(id);
}

}