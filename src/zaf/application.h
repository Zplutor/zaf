#pragma once

#include <Windows.h>
#include <filesystem>
#include <memory>
#include <set>
#include <system_error>
#include <zaf/application_event_infos.h>
#include <zaf/config.h>
#include <zaf/rx/subject/single_subject.h>
#include <zaf/rx/disposable_host.h>

namespace zaf::rx::internal {
class RxRuntime;
}

namespace zaf::testing {
class WindowTest;
}

namespace zaf {
namespace crypto::internal {
class CryptoManager;
}

namespace internal {
class SystemMessageWindow;
class WindowLifecycleFacet;
class WindowVisibilityFacet;
}

namespace wic {
class ImagingFactory;
}

class ApplicationDelegate;
class GraphicFactory;
class ResourceFactory;
class ScreenManager;
class URILoader;
class Window;
class WindowClassRegistry;
class WindowHolder;

class InitializationOptions {
public:
    std::shared_ptr<ApplicationDelegate> delegate;
    HICON window_icon{};
    HICON window_small_icon{};
    std::shared_ptr<URILoader> custom_uri_loader;
};

/**
A singleton class representing the zaf runtime, which manages the application's lifecycle, and 
provides access global shared objects and resources.

@details 
    Before using any facilities of zaf, users must call Initialize() to initialize the runtime. 
    Typically, this is done at the entry point of the application. 

    Users can implement the ApplicationDelegate interface and pass the object into the runtime via 
    Initialize(). The interface methods will be called at certain times during the application's 
    lifecycle. Such as when the application begins to run and is about to end.

    After initialization, users can call Run() to start the application running. Users are 
    responsible for creating and showing windows. This can be done either in the
    ApplicationDelegate::OnApplicationStarted() or by subscribing to the StartedEvent().

    A window can be set as the main window of the application by calling SetMainWindow(). Once the 
    main window is destroyed, the application ends automatically. Users also can call Exit()
    to end the application manually.
*/
class Application : public rx::DisposableHost, NonCopyableNonMovable {
public:
    /**
    Gets the singleton instance.
    */
    static Application& Instance();

public:
    /**
    Initializes the application runtime.

    @param options
        The initialization options.

    @pre 
        The application runtime hasn't been initialized.

    @throw PreconditionError
        Thrown if the precondition is violated.

    @throw Error
        Thrown if there are errors during initialization.

    @details
        This method should be called to initialize the runtime at the entry point of the 
        application, before using any facilities of zaf.

        If initialization fails, the application can not continue to run and should exit.
    */
    void Initialize(const InitializationOptions& options);

    /**
    Runs the application by entering a main message loop.

    @pre 
        The application runtime has been initialized.

    @throw PreconditionError
        Thrown if the precondition is violated.

    @throw ...
        Any exceptions that are thrown by:
        - ApplicationDelegate::OnApplicationStarted() or subscriptions of StartedEvent();
        - User codes during the handling of the main message loop.
        - ApplicationDelegate::OnApplicationExiting() or subscriptions of ExitingEvent();

    @details
        This method will not return until the message loop ends.

        Before the message loop starts, ApplicationDelegate::OnApplicationStarted() is called, and then
        StartedEvent() is raised. After the message loop ends, ApplicationDelegate::OnApplicationExiting() is 
        called, and then ExitingEvent() is raised.

        To end the main message loop, users can either close the main window or call Exit().
     */
    void Run();

    /**
    Exits the application.

    @details
        This method destroys all windows and ends the main message loop.
    */
    void Exit();

    /**
    Gets the ApplicationDelegate object that is passed into the application runtime when calling 
    Initialize().

    @return
        The ApplicationDelegate object. It might be nullptr if no such object is set when calling 
        Initialize().
    */
    const std::shared_ptr<ApplicationDelegate>& Delegate() const noexcept {
        return delegate_;
    }

    std::filesystem::path GetWorkingDirectoryPath() const;
    std::filesystem::path GetExeFilePath() const;
    std::filesystem::path GetExeDirectoryPath() const;

    ResourceFactory& GetResourceFactory() const {
        return *resource_factory_;
    }

    GraphicFactory& GetGraphicFactory() const {
        return *graphic_factory_;
    }

    wic::ImagingFactory& GetImagingFactory() const {
        return *imaging_factory_;
    }

    WindowClassRegistry& GetWindowClassRegistry() const {
        return *window_class_registry_;
    }

    ScreenManager& GetScreenManager() const noexcept {
        return *screen_manager_;
    }

    /**
     Get the system DPI.
     */
    float GetSystemDPI() const;

    /**
    Gets the event that is raised after the application has started running.

    @details
        Users can perform startup tasks at this event, such as creating and showing the main
        window.

        This event should be subscribed before calling Run().

        This event is raised before calling ApplicationDelegate::OnApplicationStarted().
    */
    rx::Single<ApplicationStartedInfo> StartedEvent() const noexcept;

    /**
    Gets the event that is raised when the application is about to exit.

    @details
        Users can perform cleanup tasks at this event.

        This event is raised before calling ApplicationDelegate::OnApplicationExiting().
    */
    rx::Single<ApplicationExitingInfo> ExitingEvent() const noexcept;

    /**
    Gets the event that is raised when the system user session is about to end.

    @details
        This event is raised before calling ApplicationDelegate::OnSystemSessionEnding().
    */
    rx::Single<SystemSessionEndingInfo> SystemSessionEndingEvent() const noexcept;

    /**
    Gets the main window of the application.

    @return
        The main window of the application. It might be nullptr if no main window is set.
    */
    std::shared_ptr<Window> MainWindow() const noexcept {
        return main_window_.lock();
    }

    /**
    Sets the main window for the application.

    @param window
        The window to set. It can be nullptr to cancel setting the main window. Despite this
        parameter being defined as std::shared_ptr, the application won't share ownership of the 
        window; users must manage the window's lifecycle themselves.

    @details
        The application will terminate automatically after the main window is destroyed.
    */
    void SetMainWindow(std::shared_ptr<Window> window) noexcept;
    
private:
    friend class Window;
    friend class internal::WindowLifecycleFacet;
    friend class internal::WindowVisibilityFacet;
    friend class zaf::testing::WindowTest;

    void RegisterShownWindow(const std::shared_ptr<WindowHolder>& window_holder);
    void UnregisterShownWindow(const std::shared_ptr<WindowHolder>& window_holder);
    bool IsWindowRegistered(const std::shared_ptr<Window>& window) const noexcept;

private:
    friend class rx::internal::RxRuntime;
    rx::internal::RxRuntime& GetRxRuntime() const {
        return *rx_runtime_;
    }

    friend class crypto::internal::CryptoManager;
    crypto::internal::CryptoManager& CryptoManager();

private:
    Application();
    ~Application();

    void InitializeSystemMessageWindow();

    void RaiseStartedEvent();
    void RaiseExitingEvent();

    void DestroyAllWindows();

private:
    bool is_initialized_;

    std::unique_ptr<rx::internal::RxRuntime> rx_runtime_;
    std::unique_ptr<ResourceFactory> resource_factory_;
    std::unique_ptr<GraphicFactory> graphic_factory_;
    std::unique_ptr<wic::ImagingFactory> imaging_factory_;
    std::unique_ptr<WindowClassRegistry> window_class_registry_;
    std::shared_ptr<internal::SystemMessageWindow> system_message_window_;
    std::unique_ptr<ScreenManager> screen_manager_;
    std::weak_ptr<Window> main_window_;
    std::set<std::shared_ptr<WindowHolder>> window_holders_;

    std::unique_ptr<crypto::internal::CryptoManager> crypto_manager_;
    std::once_flag crypto_manager_once_flag_;

    std::shared_ptr<ApplicationDelegate> delegate_;

    rx::SingleSubject<ApplicationStartedInfo> started_event_;
    rx::SingleSubject<ApplicationExitingInfo> exiting_event_;
    rx::SingleSubject<SystemSessionEndingInfo> system_session_ending_event_;
};

}