#include <zaf/rx/internal/subscription/inner_subscription_holder.h>
#include <zaf/base/container/utility/erase.h>

namespace zaf::internal {

InnerSubscriptionHolder::~InnerSubscriptionHolder() {
    Clear();
}


void InnerSubscriptionHolder::Add(
    const std::shared_ptr<SubscriptionCore>& subscription) {

    auto notification_id = subscription->RegisterFinishNotification(std::bind(
        &InnerSubscriptionHolder::OnNoTagSubscriptionFinish, 
        this, 
        std::placeholders::_1,
        std::placeholders::_2));

    if (!notification_id) {
        return;
    }

    std::scoped_lock<std::mutex> lock(lock_);
    no_tag_items_.push_back({ subscription, *notification_id });
}


void InnerSubscriptionHolder::OnNoTagSubscriptionFinish(
    SubscriptionCore* core, 
    int notification_id) {

    std::scoped_lock<std::mutex> lock(lock_);

    EraseIf(no_tag_items_, [core, notification_id](const auto& item) {
        
        return 
            (item.subscription.get() == core) && 
            (item.finish_notification_id == notification_id);
    });
}


void InnerSubscriptionHolder::Add(
    const std::string& tag,
    const std::shared_ptr<SubscriptionCore>& subscription) {

    auto notification_id = subscription->RegisterFinishNotification(std::bind(
        &InnerSubscriptionHolder::OnIdSubscriptionFinish, 
        this, 
        std::placeholders::_1,
        std::placeholders::_2));

    if (!notification_id) {
        return;
    }

    Item item{ subscription, *notification_id };

    std::scoped_lock<std::mutex> lock(lock_);

    auto iterator = tag_items_.find(tag);
    if (iterator == tag_items_.end()) {
        tag_items_[tag] = std::move(item);
    }
    else {
        UnregisterItemNotification(iterator->second);
        iterator->second = std::move(item);
    }
}


void InnerSubscriptionHolder::OnIdSubscriptionFinish(
    SubscriptionCore* core, 
    int notification_id) {

    std::scoped_lock<std::mutex> lock(lock_);

    auto iterator = tag_items_.begin();
    while (iterator != tag_items_.end()) {

        if ((iterator->second.subscription.get() == core) && 
            (iterator->second.finish_notification_id = notification_id)) {

            iterator = tag_items_.erase(iterator);
        }
        else {
            ++iterator;
        }
    }
}


void InnerSubscriptionHolder::Remove(const std::string& id) {

    std::scoped_lock<std::mutex> lock(lock_);

    auto iterator = tag_items_.find(id);
    if (iterator != tag_items_.end()) {

        UnregisterItemNotification(iterator->second);
        tag_items_.erase(iterator);
    }
}


void InnerSubscriptionHolder::Clear() {

    std::scoped_lock<std::mutex> lock(lock_);

    for (const auto& each_pair : tag_items_) {
        UnregisterItemNotification(each_pair.second);
    }
    tag_items_.clear();

    for (const auto& each_item : no_tag_items_) {
        UnregisterItemNotification(each_item);
    }
    no_tag_items_.clear();
}


void InnerSubscriptionHolder::UnregisterItemNotification(const Item& item) {

    item.subscription->UnregisterFinishNotification(item.finish_notification_id);
}

}