#include <zaf/application.h>
#include <dwrite.h>
#include <zaf/base/assert.h>
#include <zaf/graphic/renderer_factory.h>
#include <zaf/window/window.h>

namespace zaf {

Application& Application::GetInstance() {
	static Application instance;
	return instance;
}


Application::Application() : is_initialized_(false) {

}


bool Application::Initialize() {

	if (is_initialized_) {
		return true;
	}

	HRESULT result = CoInitialize(nullptr);
	if (FAILED(result)) {
		return false;
	}

	if (! Window::RegisterDefaultClass()) {
		return false;
	}

	ID2D1Factory* d2d_factory_handle = nullptr;
	result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d_factory_handle);
	if (FAILED(result)) {
		return false;
	}

	IDWriteFactory* dwrite_factory_handle = nullptr;
	result = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&dwrite_factory_handle)
	);
	if (FAILED(result)) {
		d2d_factory_handle->Release();
		return false;
	}

	renderer_factory_ = std::make_shared<RendererFactory>(d2d_factory_handle, dwrite_factory_handle);
	is_initialized_ = true;
	return true;
}


void Application::Run() {

	if (! is_initialized_) {
		ZAF_FAIL();
		return;
	}

	begin_run_event_.Trigger(*this);

	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	end_run_event_.Trigger(*this);
}


void Application::RegisterWindow(const std::shared_ptr<Window>& window) {

	windows_.insert(window);
	window->OnClose.AddListener(std::bind(&Application::WindowClosed, this, std::placeholders::_1));
}


void Application::WindowClosed(const std::shared_ptr<Window>& window) {

	windows_.erase(window);
}

}