#include <zaf/application.h>
#include <atlbase.h>
#include <dwrite.h>
#include <zaf/application_delegate.h>
#include <zaf/base/error/com_error.h>
#include <zaf/base/error/precondition_error.h>
#include <zaf/creation.h>
#include <zaf/crypto/internal/crypto_manager.h>
#include <zaf/graphic/wic/imaging_factory.h>
#include <zaf/graphic/graphic_factory.h>
#include <zaf/internal/message_loop.h>
#include <zaf/internal/system_message_window.h>
#include <zaf/resource/resource_factory.h>
#include <zaf/rx/internal/rx_runtime.h>
#include <zaf/window/screen_manager.h>
#include <zaf/window/window.h>
#include <zaf/window/window_class_registry.h>
#include <zaf/window/window_holder.h>

namespace zaf {

Application& Application::Instance() {
    static Application instance;
    return instance;
}


Application::Application() : is_initialized_(false) {

}


Application::~Application() {

    //Make sure that all windows are destroyed before exiting application.
    assert(window_holders_.empty());
}


void Application::Initialize(const InitializationOptions& parameters) {

    ZAF_EXPECT(!is_initialized_);

    rx_runtime_ = std::make_unique<rx::internal::RxRuntime>();

    resource_factory_.reset(new ResourceFactory(parameters.custom_uri_loader));

    HRESULT result = OleInitialize(nullptr);
    ZAF_THROW_IF_COM_ERROR(result);

    //Create Direct2D factory.
    COMPtr<ID2D1Factory> d2d_factory_handle;
    result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2d_factory_handle.Reset());
    ZAF_THROW_IF_COM_ERROR(result);

    //Create DWrite factory.
    COMPtr<IDWriteFactory> dwrite_factory_handle;
    result = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(dwrite_factory_handle.Reset()));

    ZAF_THROW_IF_COM_ERROR(result);

    //Create WIC imaging factory
    COMPtr<IWICImagingFactory> imaging_factory_handle;
    result = CoCreateInstance(
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        __uuidof(IWICImagingFactory),
        imaging_factory_handle.ResetAsVoid());

    ZAF_THROW_IF_COM_ERROR(result);

    graphic_factory_.reset(new GraphicFactory(
        d2d_factory_handle.Detach(),
        dwrite_factory_handle.Detach()));

    imaging_factory_.reset(new wic::ImagingFactory(imaging_factory_handle));

    window_class_registry_.reset(new WindowClassRegistry(
        parameters.window_icon, 
        parameters.window_small_icon));

    screen_manager_.reset(new ScreenManager());

    InitializeSystemMessageWindow();
    delegate_ = parameters.delegate;

    is_initialized_ = true;
}


void Application::InitializeSystemMessageWindow() {

    system_message_window_ = std::make_unique<internal::SystemMessageWindow>();

    auto task_bar_created_message_id = RegisterWindowMessage(L"TaskbarCreated");

    Disposables() += system_message_window_->MessageReceivedEvent().Subscribe(
        [this, task_bar_created_message_id](const Message& message) {
    
            if (message.ID() == task_bar_created_message_id) {
                auto event_observer = system_taskbar_created_event_.AsObserver();
                event_observer.OnNext(SystemTaskbarCreatedInfo{});
                if (delegate_) {
                    delegate_->OnSystemTaskbarCreated(SystemTaskbarCreatedInfo{});
                }
            }
            else if (message.ID() == WM_TIMECHANGE) {
                auto event_observer = system_time_changed_event_.AsObserver();
                event_observer.OnNext(SystemTimeChangedInfo{});
                if (delegate_) {
                    delegate_->OnSystemTimeChanged(SystemTimeChangedInfo{});
                }
            }
            else if (message.ID() == WM_ENDSESSION) {
                auto event_observer = system_session_ending_event_.AsObserver();
                event_observer.OnSuccess(SystemSessionEndingInfo{});
                if (delegate_) {
                    delegate_->OnSystemSessionEnding(SystemSessionEndingInfo{});
                }
            }
        });
}


void Application::Run() {

    ZAF_EXPECT(is_initialized_);

    RaiseStartedEvent();

    internal::MessageLoop message_loop;
    message_loop.Run();

    window_holders_.clear();

    RaiseExitingEvent();
}


void Application::RaiseStartedEvent() {

    ApplicationStartedInfo event_info;
    auto event_observer = started_event_.AsObserver();
    event_observer.OnSuccess(event_info);
    
    if (delegate_) {
        delegate_->OnApplicationStarted(event_info);
    }
}


void Application::RaiseExitingEvent() {

    ApplicationExitingInfo event_info;
    auto event_observer = exiting_event_.AsObserver();
    event_observer.OnSuccess(event_info);

    if (delegate_) {
        delegate_->OnApplicationExiting(event_info);
    }
}


void Application::Exit() {

    DestroyAllWindows();
    PostQuitMessage(0);
}


void Application::DestroyAllWindows() {

    //Copy is needed because windows_ will be modified during closing windows.
    auto all_holders = window_holders_;
    for (const auto& each_holder : all_holders) {

        const auto& window = each_holder->Window();
        if (window) {
            window->Destroy();
        }
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


rx::Single<ApplicationStartedInfo> Application::StartedEvent() const noexcept {
    return started_event_.AsSingle();
}


rx::Single<ApplicationExitingInfo> Application::ExitingEvent() const noexcept {
    return exiting_event_.AsSingle();
}


rx::Observable<SystemTaskbarCreatedInfo> Application::SystemTaskbarCreatedEvent() const noexcept {
    return system_taskbar_created_event_.AsObservable();
}


rx::Observable<SystemTimeChangedInfo> Application::SystemTimeChangedEvent() const noexcept {
    return system_time_changed_event_.AsObservable();
}


rx::Single<SystemSessionEndingInfo> Application::SystemSessionEndingEvent() const noexcept {
    return system_session_ending_event_.AsSingle();
}


void Application::RegisterShownWindow(const std::shared_ptr<WindowHolder>& window_holder) {

    ZAF_EXPECT(window_holder->Window());

    window_holders_.insert(window_holder);
}


void Application::UnregisterShownWindow(const std::shared_ptr<WindowHolder>& window_holder) {

    window_holders_.erase(window_holder);

    auto main_window = main_window_.lock();
    if (main_window == window_holder->Window()) {
        Exit();
    }
}


bool Application::IsWindowRegistered(const std::shared_ptr<Window>& window) const noexcept {

    for (const auto& each_holder : window_holders_) {
        if (each_holder->Window() == window) {
            return true;
        }
    }
    return false;
}


void Application::SetMainWindow(std::shared_ptr<Window> window) noexcept {
    main_window_ = std::move(window);
}


crypto::internal::CryptoManager& Application::CryptoManager() {

    std::call_once(crypto_manager_once_flag_, [this]() {
        crypto_manager_ = std::make_unique<crypto::internal::CryptoManager>();
    });
    return *crypto_manager_;
}

}