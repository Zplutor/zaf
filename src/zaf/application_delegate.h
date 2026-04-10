#pragma once

#include <zaf/application_event_infos.h>
#include <zaf/base/non_copyable.h>

namespace zaf {

/**
An interface for user code to handle specific notifications during the application lifecycle.

@details
    Users implement this interface and pass an instance into the application runtime when calling 
    Application::Initialize().
*/
class ApplicationDelegate : NonCopyableNonMovable {
public:
    ApplicationDelegate() = default;
    virtual ~ApplicationDelegate() = default;

    /**
    Called when the application has started running.
    
    @details
        Users can perform startup tasks at this event, such as creating and showing the main
        window.

        This methods is called after raising Application::StartedEvent().
    */
    virtual void OnApplicationStarted(const ApplicationStartedInfo&) { }

    /**
    Called when the application is about to exit.

    @details
        Users can perform cleanup tasks at this event.

        This methods is called after raising Application::ExitingEvent().
    */
    virtual void OnApplicationExiting(const ApplicationExitingInfo&) { }

    /**
    Called when the system taskbar is created.

    @details
        This method is called after raising Application::SystemTaskbarCreatedEvent().
    */
    virtual void OnSystemTaskbarCreated(const SystemTaskbarCreatedInfo&) { }

    /**
    Called when the system time is changed.

    @details
        This method is called after raising Application::SystemTimeChangedEvent().
    */
    virtual void OnSystemTimeChanged(const SystemTimeChangedInfo&) { }

    /**
    Called when the system user session is about to end.

    @details
        This method is called when the application receives the WM_ENDSESSION message.

        This method is called after raising Application::SystemSessionEndingEvent().
    */
    virtual void OnSystemSessionEnding(const SystemSessionEndingInfo&) { }
};

}