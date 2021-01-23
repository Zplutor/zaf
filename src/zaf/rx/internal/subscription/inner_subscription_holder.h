#pragma once

#include <map>
#include <mutex>
#include <vector>
#include <zaf/rx/internal/subscription/inner_subscription.h>
#include <zaf/rx/internal/subscription/subscription_core.h>

namespace zaf::internal {

class InnerSubscriptionHolder {
public:
    InnerSubscriptionHolder() = default;
    ~InnerSubscriptionHolder();

    InnerSubscriptionHolder(const InnerSubscriptionHolder&) = delete;
    InnerSubscriptionHolder& operator=(const InnerSubscriptionHolder&) = delete;

    void Add(const std::shared_ptr<InnerSubscription>& subscription);

    void Add(
        const std::string& tag, 
        const std::shared_ptr<InnerSubscription>& subscription);

    void Remove(const std::string& tag);

    void Clear();

private:
    class Item {
    public:
        std::shared_ptr<InnerSubscription> subscription;
        int finish_notification_id{};
    };

private:
    static void UnregisterItemNotification(const Item& item);

private:
    void OnNoTagSubscriptionFinish(SubscriptionCore* core, int notification_id);
    void OnIdSubscriptionFinish(SubscriptionCore* core, int notification_id);

private:
    std::mutex lock_;
    std::vector<Item> no_tag_items_;
    std::map<std::string, Item> tag_items_;
};

}