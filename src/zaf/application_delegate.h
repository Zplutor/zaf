#pragma once

#include <zaf/application_event_infos.h>
#include <zaf/base/non_copyable.h>

namespace zaf {

class ApplicationDelegate : NonCopyableNonMovable {
public:
    ApplicationDelegate() = default;
    virtual ~ApplicationDelegate() = default;

    virtual void OnBeginRun(const BeginRunInfo&) = 0;
    virtual void OnEndRun(const EndRunInfo&) = 0;

    virtual void OnSessionEnd(const SessionEndInfo&) = 0;
};

}