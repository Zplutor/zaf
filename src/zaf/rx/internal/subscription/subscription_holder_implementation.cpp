#include <zaf/rx/internal/subscription/subscription_holder_implementation.h>
#include <zaf/base/container/utility/erase.h>

namespace zaf::internal {

SubscriptionHolderImplementation::~SubscriptionHolderImplementation() {
    Clear();
}


void SubscriptionHolderImplementation::Add(
    const std::shared_ptr<SubscriptionImplementation>& subscription) {

    auto core = subscription->GetCore();
    if (!core) {
        return;
    }

    auto notification_id = core->RegisterFinishNotification(std::bind(
        &SubscriptionHolderImplementation::OnNoTagSubscriptionFinish, 
        this, 
        std::placeholders::_1,
        std::placeholders::_2));

    if (!notification_id) {
        return;
    }

    std::scoped_lock<std::mutex> lock(lock_);
    no_tag_items_.push_back({ subscription, *notification_id });
}


void SubscriptionHolderImplementation::OnNoTagSubscriptionFinish(
    SubscriptionCore* core, 
    int notification_id) {

    std::scoped_lock<std::mutex> lock(lock_);

    EraseIf(no_tag_items_, [core, notification_id](const auto& item) {
        
        return 
            (item.subscription->GetCore().get() == core) && 
            (item.finish_notification_id == notification_id);
    });
}


void SubscriptionHolderImplementation::Add(
    const std::string& tag,
    const std::shared_ptr<SubscriptionImplementation>& subscription) {

    auto core = subscription->GetCore();
    if (!core) {
        return;
    }

    auto notification_id = core->RegisterFinishNotification(std::bind(
        &SubscriptionHolderImplementation::OnIdSubscriptionFinish, 
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


void SubscriptionHolderImplementation::OnIdSubscriptionFinish(
    SubscriptionCore* core, 
    int notification_id) {

    std::scoped_lock<std::mutex> lock(lock_);

    auto iterator = tag_items_.begin();
    while (iterator != tag_items_.end()) {

        if ((iterator->second.subscription->GetCore().get() == core) && 
            (iterator->second.finish_notification_id = notification_id)) {

            iterator = tag_items_.erase(iterator);
        }
        else {
            ++iterator;
        }
    }
}


void SubscriptionHolderImplementation::Remove(const std::string& id) {

    std::scoped_lock<std::mutex> lock(lock_);

    auto iterator = tag_items_.find(id);
    if (iterator != tag_items_.end()) {

        UnregisterItemNotification(iterator->second);
        tag_items_.erase(iterator);
    }
}


void SubscriptionHolderImplementation::Clear() {

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


void SubscriptionHolderImplementation::UnregisterItemNotification(const Item& item) {

    item.subscription->GetCore()->UnregisterFinishNotification(item.finish_notification_id);
}

}