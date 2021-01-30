#pragma once

#include <Windows.h>
#include <filesystem>
#include <memory>
#include <set>
#include <system_error>
#include <zaf/config.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/subject.h>
#include <zaf/rx/subscription_host.h>

namespace zaf {
namespace internal {
class RxRuntime;
}

namespace wic {
class ImagingFactory;
}

class ApplicationBeginRunInfo;
class ApplicationEndRunInfo;
class GraphicFactory;
class ReflectionManager;
class ResourceManager;
class UriLoader;
class Window;

class InitializeParameters {
public:
	std::shared_ptr<UriLoader> relative_uri_loader;
};

/**
 Represents the application runtime instance.

 Application manages the execution, and maintains global objects.

 There is only one instance of Application, which can use Instance method to get.
 */
class Application : public SubscriptionHost {
public:
	/**
	 Get the singleton instance.
	 */
	static Application& Instance();

public:
	/**
	 Initialize the application.

	 @param parameters
		Initialization parameters.

	 This mehtod must be called before calling Run method. If the initialization
	 fails, the application is unable to run, it should be terminated in this case.
	 */
	void Initialize(const InitializeParameters& parameters);

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

	std::filesystem::path GetWorkingDirectoryPath() const;
	std::filesystem::path GetExeDirectoryPath() const;

    ReflectionManager& GetReflectionManager() const {
        return *reflection_manager_;
    }

	ResourceManager& GetResourceManager() const {
		return *resource_manager_;
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

    /**
     Get the current DPI.

     Initialize method must be called before calling this method.

     @return 
        The first value is the horizontal DPI; and the second is the vertical DPI.
     */
    std::pair<float, float> GetDpi() const;

	/**
	 Get the application began run event.

     Startup works can be done in this event, Such as creating and showing the main window.
	 */
	Observable<ApplicationBeginRunInfo> BeginRunEvent() {
		return begin_run_event_.GetObservable();
	}

	/**
	 Get the application will end run event.

     Cleanup works can be done in this event.
	 */
	Observable<ApplicationEndRunInfo> EndRunEvent() {
		return end_run_event_.GetObservable();
	}

    /**
     Get the main window.
     */
    const std::shared_ptr<Window>& GetMainWindow() const {
        return main_window_;
    }

    /**
     Set the main window.

     The application would terminate after the main window closed.

     The main window can be nullptr.
     */
    void SetMainWindow(const std::shared_ptr<Window>& window);
	
private:
	friend class Window;

	void RegisterWindow(const std::shared_ptr<Window>& window);
    void UnregisterWindow(const std::shared_ptr<Window>& window);

private:
	friend class internal::RxRuntime;

	internal::RxRuntime& GetRxRuntime() const {
		return *rx_runtime_;
	}

private:
	Application();

	void MainWindowClosed();

	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

private:
	bool is_initialized_;

	std::unique_ptr<internal::RxRuntime> rx_runtime_;
    std::unique_ptr<ReflectionManager> reflection_manager_;
	std::unique_ptr<ResourceManager> resource_manager_;
    std::unique_ptr<GraphicFactory> graphic_factory_;
	std::unique_ptr<wic::ImagingFactory> imaging_factory_;
    std::shared_ptr<Window> main_window_;
	Subscription main_window_subscription_;
	std::set<std::shared_ptr<Window>> windows_;

	Subject<ApplicationBeginRunInfo> begin_run_event_;
	Subject<ApplicationEndRunInfo> end_run_event_;
};


class ApplicationBeginRunInfo {

};


class ApplicationEndRunInfo {

};


inline Application& GetApplication() {
    return Application::Instance();
}


inline ReflectionManager& GetReflectionManager() {
    return Application::Instance().GetReflectionManager();
}


inline ResourceManager& GetResourceManager() {
	return Application::Instance().GetResourceManager();
}


inline GraphicFactory& GetGraphicFactory() {
    return GetApplication().GetGraphicFactory();
}


inline wic::ImagingFactory& GetImagingFactory() {
	return GetApplication().GetImagingFactory();
}

}