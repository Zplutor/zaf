#pragma once

#include <Windows.h>
#include <filesystem>
#include <memory>
#include <set>
#include <system_error>
#include <zaf/application_event_infos.h>
#include <zaf/config.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/subject.h>
#include <zaf/rx/subscription_host.h>

namespace zaf {
namespace crypto::internal {
class CryptoManager;
}

namespace internal {
class RxRuntime;
class SystemMessageWindow;
}

namespace wic {
class ImagingFactory;
}

class ApplicationDelegate;
class GraphicFactory;
class ResourceFactory;
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
    lifecyle. Such as when the application begins to run and is about to end.

    After initialization, users can call Run() to start the application running. Users are 
    responsible for creating and showing windows. This can be done either in the
    ApplicationDelegate::OnBeginRun() or by subscribing to the BeginRunEvent().

    A window can be set as the main window of the application by calling SetMainWindow(). Once the 
    main window is destroyed, the application ends automatically. Users also can call Terminate()
    to end the application manually.
*/
class Application : public SubscriptionHost, NonCopyableNonMovable {
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
        The application runtime is not initialized.

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
     Make the application run.

     Before calling this method, Initialize method must be called, and it must succeeds.

     The application enters a message loop while running, so this method would not return
     until the message loop ends. 
     
     OnBeginRun event is triggered after the running began. OnEndRun event is triggered 
     before the running end.
     */
    void Run();

    /**
     Terminate the application.

     After calling this method, the message loop ends, and the application exits normally.
     */
    void Terminate();

    const std::shared_ptr<ApplicationDelegate>& GetDelegate() const {
        return delegate_;
    }

    std::filesystem::path GetWorkingDirectoryPath() const;
    std::filesystem::path GetExeFilePath() const;
    std::filesystem::path GetExeDirectoryPath() const;

    ResourceFactory& GetResourceFactory() const {
        return *resource_factory_;
    }

    /**
     Get the resource factory.
     */
    GraphicFactory& GetGraphicFactory() const {
        return *graphic_factory_;
    }

    wic::ImagingFactory& GetImagingFactory() const {
        return *imaging_factory_;
    }

    WindowClassRegistry& GetWindowClassRegistry() const {
        return *window_class_registry_;
    }

    /**
     Get the system DPI.
     */
    float GetSystemDPI() const;

    /**
    Gets the event that will be raised when the application begins to run.

    @details
        Users can perform startup tasks at this event, such as creating and showing the main
        window.

        This event is raised after ApplicationDelegate::OnBeginRun() is called.
    */
    Observable<BeginRunInfo> BeginRunEvent() const {
        return begin_run_event_.AsObservable();
    }

    /**
    Gets the event that will be raised when the application is about to end.

    @details
        Users can perform cleanup tasks at this event.

        This event is raised after ApplicationDelegate::OnEndRun() is called.
    */
    Observable<EndRunInfo> EndRunEvent() const {
        return end_run_event_.AsObservable();
    }

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
        window; users must manage the window's lifecyle themselves.

    @details
        The application will terminate automatically after the main window is destroyed.
    */
    void SetMainWindow(std::shared_ptr<Window> window) noexcept;
    
private:
    friend class Window;

    void RegisterShownWindow(const std::shared_ptr<WindowHolder>& window_holder);
    void UnregisterShownWindow(const std::shared_ptr<WindowHolder>& window_holder);

private:
    friend class internal::RxRuntime;
    internal::RxRuntime& GetRxRuntime() const {
        return *rx_runtime_;
    }

    friend class crypto::internal::CryptoManager;
    crypto::internal::CryptoManager& CryptoManager();

private:
    Application();
    ~Application();

    void InitializeSystemMessageWindow();

    void NotifyApplicationBeginRun();
    void NotifyApplicationEndRun();

    void DestroyAllWindows();

private:
    bool is_initialized_;

    std::unique_ptr<internal::RxRuntime> rx_runtime_;
    std::unique_ptr<ResourceFactory> resource_factory_;
    std::unique_ptr<GraphicFactory> graphic_factory_;
    std::unique_ptr<wic::ImagingFactory> imaging_factory_;
    std::unique_ptr<WindowClassRegistry> window_class_registry_;
    std::shared_ptr<internal::SystemMessageWindow> system_message_window_;
    std::weak_ptr<Window> main_window_;
    std::set<std::shared_ptr<WindowHolder>> window_holders_;

    std::unique_ptr<crypto::internal::CryptoManager> crypto_manager_;
    std::once_flag crypto_manager_once_flag_;

    std::shared_ptr<ApplicationDelegate> delegate_;

    Subject<BeginRunInfo> begin_run_event_;
    Subject<EndRunInfo> end_run_event_;
};

}