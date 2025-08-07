#include <zaf/rx/subscription_bag.h>
#include <zaf/base/container/utility/erase.h>
#include <zaf/rx/internal/subscription/subscription_core.h>

namespace zaf::rx {

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

    auto notification_id = core->RegisterDisposeNotification(std::bind(
        &SubscriptionBag::OnSubscriptionCoreDispose,
        this,
        std::placeholders::_1));

    if (!notification_id) {
        return;
    }

    std::scoped_lock<std::mutex> lock(lock_);
    items_.push_back({ core, *notification_id });
}


void SubscriptionBag::OnSubscriptionCoreDispose(
    rx::internal::DisposeNotificationID notification_id) {

    std::scoped_lock<std::mutex> lock(lock_);

    EraseIf(items_, [notification_id](const auto& item) {
        return (item.dispose_notification_id == notification_id);
    });
}


void SubscriptionBag::operator+=(const Subscription& subscription) {
    Add(subscription);
}


void SubscriptionBag::Clear() {

    std::scoped_lock<std::mutex> lock(lock_);

    for (const auto& each_item : items_) {

        const auto& core = each_item.subscription_core;
        core->UnregisterDisposeNotification(each_item.dispose_notification_id);
        core->Dispose();
    }

    items_.clear();
}


std::size_t SubscriptionBag::Count() const {
    std::scoped_lock<std::mutex> lock(lock_);
    return items_.size();
}

}