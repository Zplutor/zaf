#pragma once

#include <zaf/application_event_infos.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/subject.h>
#include <zaf/rx/subscription_host.h>
#include <zaf/window/internal/inner_message_only_window.h>

namespace zaf::internal {

class SystemMessageWindow : public SubscriptionHost {
public:
    SystemMessageWindow();

    Observable<SessionEndInfo> SessionEndedEvent() {
        return session_ended_subject_.AsObservable();
    }

private:
    InnerMessageOnlyWindow message_only_window_;
    Subject<SessionEndInfo> session_ended_subject_;
};

}