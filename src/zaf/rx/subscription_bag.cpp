#include <zaf/rx/subscription_bag.h>
#include <zaf/base/container/utility/erase.h>
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

    auto notification_id = core->RegisterUnsubscribeNotification(std::bind(
        &SubscriptionBag::OnSubscriptionCoreUnsubscribe,
        this,
        std::placeholders::_1));

    if (!notification_id) {
        return;
    }

    std::scoped_lock<std::mutex> lock(lock_);
    items_.push_back({ core, *notification_id });
}


void SubscriptionBag::OnSubscriptionCoreUnsubscribe(
    rx::internal::UnsubscribeNotificationID notification_id) {

    std::scoped_lock<std::mutex> lock(lock_);

    EraseIf(items_, [notification_id](const auto& item) {
        return (item.unsubscribe_notification_id == notification_id);
    });
}


void SubscriptionBag::operator+=(const Subscription& subscription) {
    Add(subscription);
}


void SubscriptionBag::Clear() {

    std::scoped_lock<std::mutex> lock(lock_);

    for (const auto& each_item : items_) {

        const auto& core = each_item.subscription_core;
        core->UnregisterUnsubscribeNotification(each_item.unsubscribe_notification_id);
        core->Unsubscribe();
    }

    items_.clear();
}


std::size_t SubscriptionBag::Count() const {
    std::scoped_lock<std::mutex> lock(lock_);
    return items_.size();
}

}