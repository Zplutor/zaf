#pragma once

#include <mutex>
#include <unordered_set>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/disposable.h>
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
    void Clear() noexcept;

    /**
    Gets the number of subscriptions in the bag.
    */
    std::size_t Count() const noexcept;

private:
    mutable std::mutex lock_;
    std::unordered_set<std::shared_ptr<Disposable>> items_;
};

}