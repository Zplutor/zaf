#pragma once

#include <zaf/application_event_infos.h>
#include <zaf/rx/subscription_host.h>

namespace zaf {

class ApplicationDelegate : public SubscriptionHost {
public:
    ApplicationDelegate() = default;
    virtual ~ApplicationDelegate() = default;

    ApplicationDelegate(const ApplicationDelegate&) = delete;
    ApplicationDelegate& operator=(const ApplicationDelegate&) = delete;

    virtual void ApplicationBeginRun(const ApplicationBeginRunInfo&) = 0;
    virtual void ApplicationEndRun(const ApplicationEndRunInfo&) = 0;

    virtual void SessionEnded(const SessionEndedInfo&) = 0;
};

}