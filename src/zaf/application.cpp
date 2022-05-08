#include <zaf/application.h>
#include <atlbase.h>
#include <dwrite.h>
#include <zaf/application_delegate.h>
#include <zaf/base/assert.h>
#include <zaf/base/error/check.h>
#include <zaf/base/error/com_error.h>
#include <zaf/creation.h>
#include <zaf/graphic/image/wic/imaging_factory.h>
#include <zaf/graphic/graphic_factory.h>
#include <zaf/internal/message_loop.h>
#include <zaf/internal/system_message_window.h>
#include <zaf/resource/resource_manager.h>
#include <zaf/rx/internal/rx_runtime.h>
#include <zaf/window/window.h>

namespace zaf {

Application& Application::Instance() {
    static Application instance;
    return instance;
}


Application::Application() : is_initialized_(false) {

}


Application::~Application() {

    //Ensure to destroy all windows when exiting applications.
    DestroyAllWindows();
}


void Application::Initialize(const InitializeParameters& parameters) {

    if (is_initialized_) {
        return;
    }

    rx_runtime_ = std::make_unique<internal::RxRuntime>();

    resource_manager_ = std::make_unique<ResourceManager>(parameters.custom_uri_loader);

    HRESULT result = CoInitialize(nullptr);
    ZAF_THROW_IF_COM_ERROR(result);

    Window::RegisterDefaultClass(parameters.window_icon, parameters.window_small_icon);

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

    InitializeSystemMessageWindow();
    delegate_ = parameters.delegate;

    is_initialized_ = true;
}


void Application::InitializeSystemMessageWindow() {

    system_message_window_ = Create<internal::SystemMessageWindow>();

    Subscriptions() += system_message_window_->SessionEndedEvent().Subscribe(
        [this](const SessionEndedInfo& event_info) {
    
            if (delegate_) {
                delegate_->SessionEnded(event_info);
            }
        }
    );
}


void Application::Run() {

    ZAF_EXPECT(is_initialized_);

    NotifyApplicationBeginRun();

    internal::MessageLoop message_loop;
    message_loop.Run();

    windows_.clear();

    NotifyApplicationEndRun();
}


void Application::NotifyApplicationBeginRun() {

    ApplicationBeginRunInfo event_info;
    
    if (delegate_) {
        delegate_->ApplicationBeginRun(event_info);
    }

    auto begin_run_event_observer = begin_run_event_.GetObserver();
    begin_run_event_observer.OnNext(event_info);
    begin_run_event_observer.OnCompleted();
}


void Application::NotifyApplicationEndRun() {

    ApplicationEndRunInfo event_info;

    if (delegate_) {
        delegate_->ApplicationEndRun(event_info);
    }

    auto end_run_event_observer = end_run_event_.GetObserver();
    end_run_event_observer.OnNext(event_info);
    end_run_event_observer.OnCompleted();
}


void Application::Terminate() {

    DestroyAllWindows();
    PostQuitMessage(0);
}


void Application::DestroyAllWindows() {

    //Copy is needed because windows_ will be modified during closing windows.
    auto all_windows = windows_;
    for (const auto& each_window : all_windows) {
        each_window->Destroy();
    }
}


std::filesystem::path Application::GetWorkingDirectoryPath() const {

    wchar_t buffer[MAX_PATH]{};
    GetCurrentDirectory(MAX_PATH, buffer);

    return std::filesystem::path{ buffer };
}


std::filesystem::path Application::GetExeFilePath() const {

    wchar_t buffer[MAX_PATH]{};
    GetModuleFileName(nullptr, buffer, MAX_PATH);
    return buffer;
}


std::filesystem::path Application::GetExeDirectoryPath() const {
    return GetExeFilePath().parent_path();
}


float Application::GetSystemDPI() const {
    return static_cast<float>(GetDpiForSystem());
}


void Application::RegisterWindow(const std::shared_ptr<Window>& window) {
    windows_.insert(window);
}


void Application::UnregisterWindow(const std::shared_ptr<Window>& window) {

    windows_.erase(window);

    auto main_window = main_window_.lock();
    if (main_window == window) {
        Terminate();
    }
}


void Application::SetMainWindow(const std::shared_ptr<Window>& window) {
    main_window_ = window;
}

}