#pragma once

#include <mutex>
#include <vector>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/internal/subscription/dispose_notification.h>
#include <zaf/rx/subscription.h>

namespace zaf::rx {

/**
Represents a container that disposes added subscriptions when cleared or destroyed.
*/
class SubscriptionBag : NonCopyableNonMovable {
public:
    /**
    Constructs an empty instance.
    */
    SubscriptionBag() noexcept;

    /**
    Destructs the instance, disposing all added subscriptions.
    */
    ~SubscriptionBag();

    /**
    Adds a subscription to the bag.

    @param subscription
        The subscription to add. The subscription won't be added if it is disposed.

    @throw std::bad_alloc
    */
    void Add(const Subscription& subscription);

    /**
    Adds a subscription to the bag.

    @details
        This is a convenient operator for adding a subscription to the bag.

    @see Add(const Subscription&)
    */
    void operator+=(const Subscription& subscription);

    /**
    Clears and disposes all subscriptions in the bag.
    */
    void Clear();

    /**
    Gets the number of subscriptions in the bag.
    */
    std::size_t Count() const;

private:
    class Item {
    public:
        std::shared_ptr<rx::internal::SubscriptionCore> subscription_core;
        rx::internal::DisposeNotificationID dispose_notification_id{};
    };

private:
    void OnSubscriptionCoreDispose(rx::internal::DisposeNotificationID notification_id);

private:
    mutable std::mutex lock_;
    std::vector<Item> items_;
};

}