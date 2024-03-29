#pragma once

#include <map>
#include <mutex>
#include <vector>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/internal/subscription/inner_subscription.h>

namespace zaf::internal {

class InnerSubscriptionSet : NonCopyableNonMovable {
public:
    InnerSubscriptionSet() = default;
    ~InnerSubscriptionSet();

    void Add(const std::shared_ptr<InnerSubscription>& subscription);

    void Add(
        const std::string& tag, 
        const std::shared_ptr<InnerSubscription>& subscription);

    void Remove(const std::string& tag);

    void Clear();

    std::size_t Count() const;

private:
    class Item {
    public:
        std::shared_ptr<InnerSubscription> subscription;
        int dispose_notification_id{};
    };

private:
    static void UnregisterItemNotification(const Item& item);

private:
    void OnNoTagSubscriptionDispose(Producer* producer, int notification_id);
    void OnIdSubscriptionDispose(Producer* producer, int notification_id);

private:
    mutable std::mutex lock_;
    std::vector<Item> no_tag_items_;
    std::map<std::string, Item> tag_items_;
};

}