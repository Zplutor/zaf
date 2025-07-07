#pragma once

#include <functional>
#include <zaf/base/non_copyable.h>
#include <zaf/rx/internal/subscription/unsubscribe_notification.h>

namespace zaf::rx::internal {

class SubscriptionCore : NonCopyableNonMovable {
public:
    SubscriptionCore() = default;
    virtual ~SubscriptionCore() = default;

    virtual void Unsubscribe() = 0;
    virtual bool IsUnsubscribed() const noexcept = 0;

    virtual std::optional<UnsubscribeNotificationID> RegisterUnsubscribeNotification(
        UnsubscribeNotification callback) = 0;

    virtual void UnregisterUnsubscribeNotification(UnsubscribeNotificationID id) = 0;
};

}