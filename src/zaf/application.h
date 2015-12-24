#pragma once

#include <Windows.h>
#include <dwrite.h>
#include <memory>
#include <set>
#include <zaf/base/event.h>
#include <zaf/internal/direct2d.h>

namespace zaf {

class Application {
public:
	typedef Event<Application&> BeginRunEvent;
	typedef Event<Application&> EndRunEvent;

public:
	static Application& GetInstance();

	bool Initialize();
	void Run();

	ID2D1Factory* GetD2DFactory();
	IDWriteFactory* GetDWriteFactory();

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
	ID2D1Factory* d2d_factory_;
	IDWriteFactory* dwrite_factory_;

	std::set<std::shared_ptr<Window>> windows_;

	BeginRunEvent begin_run_event_;
	EndRunEvent end_run_event_;
};

}