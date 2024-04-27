#pragma once

#include <zaf/application_event_infos.h>
#include <zaf/base/non_copyable.h>

namespace zaf {

/**
An interface for user code to handle specific notifications during the application lifecyle.

@details
    Users implement this interface and pass an instance into the application runtime when calling 
    Application::Initialize().
*/
class ApplicationDelegate : NonCopyableNonMovable {
public:
    ApplicationDelegate() = default;
    virtual ~ApplicationDelegate() = default;

    /**
    Called when the application begins to run.
    
    @details
        Users can perform startup tasks at this event, such as creating and showing the main
        window.

        This methods is called before raising Application::BeginRunEvent().
    */
    virtual void OnBeginRun(const BeginRunInfo&) = 0;

    /**
    Called when the application is about to end.

    @details
        Users can perform cleanup tasks at this event.

        This methods is called before raising Application::EndRunEvent().
    */
    virtual void OnEndRun(const EndRunInfo&) = 0;

    /**
    Called when the system user session is about to end.

    @details
        This method is called when the application receives the WM_ENDSESSION message.
    */
    virtual void OnSessionEnd(const SessionEndInfo&) = 0;
};

}