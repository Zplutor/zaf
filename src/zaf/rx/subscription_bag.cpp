#include <zaf/rx/subscription_bag.h>
#include <zaf/base/container/utility/erase.h>
#include <zaf/rx/internal/producer.h>
#include <zaf/rx/internal/subscription/subscription_core.h>

namespace zaf {

SubscriptionBag::SubscriptionBag() noexcept {

}


SubscriptionBag::~SubscriptionBag() {
    Clear();
}


void SubscriptionBag::Add(const Subscription& subscription) {

    const auto& core = subscription.Core();
    if (!core) {
        return;
    }

    const auto& producer = core->Producer();
    if (!producer) {
        return;
    }

    auto notification_id = producer->RegisterDisposeNotification(std::bind(
        &SubscriptionBag::OnProducerDispose,
        this,
        std::placeholders::_1,
        std::placeholders::_2));

    if (!notification_id) {
        return;
    }

    std::scoped_lock<std::mutex> lock(lock_);
    items_.push_back({ core, *notification_id });
}


void SubscriptionBag::OnProducerDispose(rx::internal::Producer* producer, int notification_id) {

    std::scoped_lock<std::mutex> lock(lock_);

    EraseIf(items_, [producer, notification_id](const auto& item) {
        return
            (item.subscription_core->Producer().get() == producer) &&
            (item.dispose_notification_id == notification_id);
    });
}


void SubscriptionBag::operator+=(const Subscription& subscription) {
    Add(subscription);
}


void SubscriptionBag::Clear() {

    std::scoped_lock<std::mutex> lock(lock_);

    for (const auto& each_item : items_) {

        const auto& producer = each_item.subscription_core->Producer();
        producer->UnregisterDisposeNotification(each_item.dispose_notification_id);

        each_item.subscription_core->Unsubscribe();
    }

    items_.clear();
}


std::size_t SubscriptionBag::Count() const {
    std::scoped_lock<std::mutex> lock(lock_);
    return items_.size();
}

}