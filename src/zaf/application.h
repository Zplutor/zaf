#pragma once

#include <Windows.h>
#include <memory>
#include <set>
#include <zaf/base/event.h>

namespace zaf {

class RendererFactory;
class Window;

/**
 Represents the application runtime instance.

 There is only one instance of Application, use GetInstance method to get it.
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

	 @return
	 Return true if succeeded; otherwise return false.

	 This mehtod must be called before Run method is called. If the initialization
	 is failed, the application is unable to run, it should be terminated in 
	 this case.
	 */
	bool Initialize();

	/**
	 Make the application run.

	 Before calling this method, Initialize method must be called, and it must return true.

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
	 Get the default renderer factory.

	 @return
	 Return nullptr if Initialize method is not called, or the initialization is failed.
	 */
	const std::shared_ptr<RendererFactory>& GetRendererFactory() const {
		return renderer_factory_;
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
	 */
	BeginRunEvent::Proxy GetBeginRunEvent() {
		return BeginRunEvent::Proxy(begin_run_event_);
	}

	/**
	 Get the application will end run event.
	 */
	EndRunEvent::Proxy GetEndRunEvent() {
		return EndRunEvent::Proxy(end_run_event_);
	}

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

	std::shared_ptr<RendererFactory> renderer_factory_;
    std::shared_ptr<Window> main_window_;
	std::set<std::shared_ptr<Window>> windows_;

	BeginRunEvent begin_run_event_;
	EndRunEvent end_run_event_;
};

}