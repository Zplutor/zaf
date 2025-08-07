#include <zaf/rx/internal/subscription/producer_subscription_core.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::rx::internal {

ProducerSubscriptionCore::ProducerSubscriptionCore(
    std::shared_ptr<internal::Producer> producer) noexcept
    :
    producer_(std::move(producer)) {

}


ProducerSubscriptionCore::~ProducerSubscriptionCore() {
    producer_->Dispose();
}


void ProducerSubscriptionCore::Dispose() noexcept {
    producer_->Dispose();
}


bool ProducerSubscriptionCore::IsDisposed() const noexcept {
    return producer_->IsDisposed();
}


std::optional<DisposeNotificationID> ProducerSubscriptionCore::RegisterDisposeNotification(
    DisposeNotification callback) {

    return producer_->RegisterDisposeNotification(std::move(callback));
}


void ProducerSubscriptionCore::UnregisterDisposeNotification(DisposeNotificationID id) {
    producer_->UnregisterDisposeNotification(id);
}

}