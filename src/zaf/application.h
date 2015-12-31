#pragma once

#include <Windows.h>
#include <memory>
#include <set>
#include <zaf/base/event.h>

namespace zaf {

class RendererFactory;

class Application {
public:
	typedef Event<Application&> BeginRunEvent;
	typedef Event<Application&> EndRunEvent;

public:
	static Application& GetInstance();

	void Run();

	const std::shared_ptr<RendererFactory>& GetRendererFactory() const {
		return renderer_factory_;
	}

public:
	BeginRunEvent::Proxy OnBeginRun;
	EndRunEvent::Proxy OnEndRun;
	
private:
	friend class Window;

	void RegisterWindow(const std::shared_ptr<Window>& window);

private:
	Application();

	void WindowClosed(const std::shared_ptr<Window>& window);

	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

private:
	std::shared_ptr<RendererFactory> renderer_factory_;
	std::set<std::shared_ptr<Window>> windows_;

	BeginRunEvent begin_run_event_;
	EndRunEvent end_run_event_;
};

}