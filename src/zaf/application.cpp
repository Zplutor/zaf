#include <zaf/application.h>
#include <zaf/window.h>
#include <zaf/internal/redirect_std_stream.h>
#include <zaf/internal/theme/vs2013/theme.h>

namespace zaf {


Application& Application::GetInstance() {
	static Application instance;
	return instance;
}


Application::Application() : 
	d2d_factory_(nullptr),
	dwrite_factory_(nullptr),
	begin_run_event_(),
	end_run_event_(),
	OnBeginRun(begin_run_event_),
	OnEndRun(end_run_event_) {

}


bool Application::Initialize() {

	internal::RedirectStandardStream();

	if (! SUCCEEDED(CoInitialize(nullptr))) {
		return false;
	}

	if (! SUCCEEDED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d_factory_))) {
		return false;
	}

	HRESULT result = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED, 
		__uuidof(IDWriteFactory), 
		reinterpret_cast<IUnknown**>(&dwrite_factory_)
	);

	if (! SUCCEEDED(result)) {
		return false;
	}

	if (! Window::RegisterDefaultClass()) {
		return false;
	}

	auto theme = std::make_shared<internal::theme::vs2013::Theme>();
	internal::theme::SetTheme(theme);

	return true;
}


void Application::Run() {

	begin_run_event_.Trigger(*this);

	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	end_run_event_.Trigger(*this);

	dwrite_factory_->Release();
	dwrite_factory_ = nullptr;

	d2d_factory_->Release();
	d2d_factory_ = nullptr;

	CoUninitialize();
}


void Application::RegisterWindow(const std::shared_ptr<Window>& window) {

	windows_.insert(window);
	window->OnClose.AddListener(std::bind(&Application::WindowClosed, this, std::placeholders::_1));
}


void Application::WindowClosed(const std::shared_ptr<Window>& window) {

	windows_.erase(window);
}


ID2D1Factory* Application::GetD2DFactory() {
	return d2d_factory_;
}


IDWriteFactory* Application::GetDWriteFactory() {
	return dwrite_factory_;
}


}