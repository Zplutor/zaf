#pragma once

#include <zaf/rx/disposable.h>
#include <zaf/rx/internal/subscription/dispose_notification.h>

namespace zaf::rx::internal {

class SubscriptionCore : public Disposable {
public:
    virtual std::optional<DisposeNotificationID> RegisterDisposeNotification(
        DisposeNotification callback) = 0;

    virtual void UnregisterDisposeNotification(DisposeNotificationID id) = 0;
};

}