#include <zaf/application.h>
#include <atlbase.h>
#include <dwrite.h>
#include <zaf/base/assert.h>
#include <zaf/base/error/com_error.h>
#include <zaf/graphic/image/wic/imaging_factory.h>
#include <zaf/graphic/resource_factory.h>
#include <zaf/internal/message_loop.h>
#include <zaf/reflection/reflection_manager.h>
#include <zaf/resource/resource_manager.h>
#include <zaf/window/window.h>

namespace zaf {

Application& Application::GetInstance() {
	static Application instance;
	return instance;
}


Application::Application() : 
    is_initialized_(false), 
    reflection_manager_(std::make_unique<ReflectionManager>()) {

}


void Application::Initialize(const InitializeParameters& parameters) {

	if (is_initialized_) {
		return;
	}

    resource_manager_ = std::make_unique<ResourceManager>(parameters.relative_uri_loader);

	HRESULT result = CoInitialize(nullptr);
    ZAF_THROW_IF_COM_ERROR(result);

	Window::RegisterDefaultClass();

    //Create Direct2D factory.
	CComPtr<ID2D1Factory> d2d_factory_handle;
	result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d_factory_handle);
    ZAF_THROW_IF_COM_ERROR(result);

    //Create DWrite factory.
    CComPtr<IDWriteFactory> dwrite_factory_handle;
	result = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&dwrite_factory_handle));

    ZAF_THROW_IF_COM_ERROR(result);

    //Create WIC imaging factory
    CComPtr<IWICImagingFactory> imaging_factory_handle;
    result = CoCreateInstance(
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&imaging_factory_handle));

    ZAF_THROW_IF_COM_ERROR(result);

	graphic_factory_ = std::make_unique<GraphicFactory>(
        d2d_factory_handle.Detach(),
        dwrite_factory_handle.Detach());

    imaging_factory_ = std::make_unique<wic::ImagingFactory>(imaging_factory_handle.Detach());

	is_initialized_ = true;
}


void Application::Run() {

	if (! is_initialized_) {
		ZAF_FAIL();
		return;
	}

	begin_run_event_.Trigger(*this);

    internal::MessageLoop message_loop;
    message_loop.Run();

    main_window_ = nullptr;
    windows_.clear();

	end_run_event_.Trigger(*this);
}


void Application::Terminate() {
    PostQuitMessage(0);
}


std::pair<float, float> Application::GetDpi() const {

    std::pair<float, float> dpi_pair;

    if (graphic_factory_ == nullptr) {
        return dpi_pair;
    }

    auto factory_handle = graphic_factory_->GetDirect2dFactoryHandle();
    factory_handle->ReloadSystemMetrics();
    factory_handle->GetDesktopDpi(&dpi_pair.first, &dpi_pair.second);
    return dpi_pair;
}


void Application::RegisterWindow(const std::shared_ptr<Window>& window) {
	windows_.insert(window);
}


void Application::UnregisterWindow(const std::shared_ptr<Window>& window) {
    windows_.erase(window);
}


void Application::SetMainWindow(const std::shared_ptr<Window>& window) {

    if (main_window_ != nullptr) {
        main_window_->GetCloseEvent().RemoveListenersWithTag(reinterpret_cast<std::uintptr_t>(this));
    }

    main_window_ = window;
    
    if (main_window_ != nullptr) {
        main_window_->GetCloseEvent().AddListenerWithTag(
            reinterpret_cast<std::uintptr_t>(this), 
            std::bind(&Application::MainWindowClosed, this, std::placeholders::_1));
    }
}


void Application::MainWindowClosed(const std::shared_ptr<Window>& window) {
    Terminate();
}

}