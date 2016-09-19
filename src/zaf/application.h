#pragma once

#include <Windows.h>
#include <memory>
#include <set>
#include <system_error>
#include <zaf/base/event.h>
#include <zaf/config.h>

namespace zaf {

class ResourceFactory;
class Window;

/**
 Represents the application runtime instance.

 Application manages the execution, and maintains global objects.

 There is only one instance of Application, which can use GetInstance method to get.
 */
class Application {
public:
	/**
	 Type of application begin run event.
	 */
	typedef Event<Application&> BeginRunEvent;

	/**
	 Type of application end run event.
	 */
	typedef Event<Application&> EndRunEvent;

public:
	/**
	 Get the singleton instance.
	 */
	static Application& GetInstance();

public:
	/**
	 Initialize the application.

     @param error_code
        An output parameter indicating the result of initialization. You should check
        this parameter to ensure that whether the initialization is succeeded.

	 This mehtod must be called before calling Run method. If the initialization
	 fails, the application is unable to run, it should be terminated in this case.
	 */
	void Initialize(std::error_code& error_code);

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

	/**
	 Get the resource factory.

	 @return
	    Return nullptr if Initialize method is not called, or it fails.
	 */
	const std::shared_ptr<ResourceFactory>& GetResourceFactory() const {
		return resource_factory_;
	}

    /**
     Get the current DPI.

     Initialize method must be called before calling this method.

     @return 
        The first value is the horizontal DPI; and the second is the vertical DPI.
     */
    const std::pair<float, float> GetDpi() const;

	/**
	 Get the application began run event.

     Startup works can be done in this event, Such as creating and showing the main window.
	 */
	BeginRunEvent::Proxy GetBeginRunEvent() {
		return BeginRunEvent::Proxy(begin_run_event_);
	}

	/**
	 Get the application will end run event.

     Cleanup works can be done in this event.
	 */
	EndRunEvent::Proxy GetEndRunEvent() {
		return EndRunEvent::Proxy(end_run_event_);
	}

    /**
     Get the main window.
     */
    const std::shared_ptr<Window> GetMainWindow() const {
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
	Application();

	void MainWindowClosed(const std::shared_ptr<Window>& window);

	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

private:
	bool is_initialized_;

    std::shared_ptr<ResourceFactory> resource_factory_;
    std::shared_ptr<Window> main_window_;
	std::set<std::shared_ptr<Window>> windows_;

	BeginRunEvent begin_run_event_;
	EndRunEvent end_run_event_;
};


inline Application& GetApplication() {
    return Application::GetInstance();
}


inline const std::shared_ptr<ResourceFactory> GetResourceFactory() {
    return GetApplication().GetResourceFactory();
}

}