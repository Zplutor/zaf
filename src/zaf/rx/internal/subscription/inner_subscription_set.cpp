#include <zaf/rx/internal/subscription/inner_subscription_set.h>
#include <zaf/base/container/utility/erase.h>
#include <zaf/rx/internal/producer.h>

namespace zaf::internal {

InnerSubscriptionSet::~InnerSubscriptionSet() {
    Clear();
}


void InnerSubscriptionSet::Add(
    const std::shared_ptr<InnerSubscription>& subscription) {

    auto producer = subscription->Producer();
    if (!producer) {
        return;
    }

    auto notification_id = producer->RegisterFinishNotification(std::bind(
        &InnerSubscriptionSet::OnNoTagSubscriptionFinish, 
        this, 
        std::placeholders::_1,
        std::placeholders::_2));

    if (!notification_id) {
        return;
    }

    std::scoped_lock<std::mutex> lock(lock_);
    no_tag_items_.push_back({ subscription, *notification_id });
}


void InnerSubscriptionSet::OnNoTagSubscriptionFinish(
    Producer* producer, 
    int notification_id) {

    std::scoped_lock<std::mutex> lock(lock_);

    EraseIf(no_tag_items_, [producer, notification_id](const auto& item) {
        
        return 
            (item.subscription->Producer().get() == producer) && 
            (item.finish_notification_id == notification_id);
    });
}


void InnerSubscriptionSet::Add(
    const std::string& tag,
    const std::shared_ptr<InnerSubscription>& subscription) {

    auto producer = subscription->Producer();
    if (!producer) {
        return;
    }

    auto notification_id = producer->RegisterFinishNotification(std::bind(
        &InnerSubscriptionSet::OnIdSubscriptionFinish, 
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


void InnerSubscriptionSet::OnIdSubscriptionFinish(
    Producer* producer, 
    int notification_id) {

    std::scoped_lock<std::mutex> lock(lock_);

    auto iterator = tag_items_.begin();
    while (iterator != tag_items_.end()) {

        if ((iterator->second.subscription->Producer().get() == producer) && 
            (iterator->second.finish_notification_id = notification_id)) {

            iterator = tag_items_.erase(iterator);
        }
        else {
            ++iterator;
        }
    }
}


void InnerSubscriptionSet::Remove(const std::string& id) {

    std::scoped_lock<std::mutex> lock(lock_);

    auto iterator = tag_items_.find(id);
    if (iterator != tag_items_.end()) {

        UnregisterItemNotification(iterator->second);
        tag_items_.erase(iterator);
    }
}


void InnerSubscriptionSet::Clear() {

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


void InnerSubscriptionSet::UnregisterItemNotification(const Item& item) {

    item.subscription->Producer()->UnregisterFinishNotification(item.finish_notification_id);
}


std::size_t InnerSubscriptionSet::Count() const {

    std::scoped_lock<std::mutex> lock(lock_);
    return no_tag_items_.size() + tag_items_.size();
}

}