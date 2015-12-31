#include <zaf/application.h>
#include <zaf/internal/theme/vs2013/theme.h>
#include <zaf/graphic/renderer_factory.h>
#include <zaf/window/window.h>

namespace zaf {

Application& Application::GetInstance() {
	static Application instance;
	return instance;
}


Application::Application() : 
	begin_run_event_(),
	end_run_event_(),
	OnBeginRun(begin_run_event_),
	OnEndRun(end_run_event_) {

}


void Application::Run() {

	CoInitialize(nullptr);

	Window::RegisterDefaultClass();

	renderer_factory_ = std::make_shared<RendererFactory>();

	auto theme = std::make_shared<internal::theme::vs2013::Theme>();
	internal::theme::SetTheme(theme);

	begin_run_event_.Trigger(*this);

	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	end_run_event_.Trigger(*this);

	CoUninitialize();
}


void Application::RegisterWindow(const std::shared_ptr<Window>& window) {

	windows_.insert(window);
	window->OnClose.AddListener(std::bind(&Application::WindowClosed, this, std::placeholders::_1));
}


void Application::WindowClosed(const std::shared_ptr<Window>& window) {

	windows_.erase(window);
}

}