#include <zaf/window/window.h>
#include <Windowsx.h>
#include <zaf/application.h>
#include <zaf/base/auto_reset.h>
#include <zaf/base/error/win32_error.h>
#include <zaf/creation.h>
#include <zaf/graphic/pixel_snapping.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/dpi.h>
#include <zaf/graphic/graphic_factory.h>
#include <zaf/internal/control/keyboard_event_routing.h>
#include <zaf/internal/control/mouse_event_routing.h>
#include <zaf/internal/tab_stop_utility.h>
#include <zaf/internal/theme.h>
#include <zaf/internal/window/window_focused_control_manager.h>
#include <zaf/window/inspector/inspector_window.h>
#include <zaf/window/internal/window_style_shim.h>
#include <zaf/window/tooltip_window.h>
#include <zaf/window/invalid_handle_state_error.h>
#include <zaf/window/message/hit_test_message.h>
#include <zaf/window/message/keyboard_message.h>
#include <zaf/window/message/message.h>
#include <zaf/window/message/mouse_message.h>
#include <zaf/window/screen_manager.h>
#include <zaf/window/window_class_registry.h>
#include <zaf/window/window_holder.h>

namespace zaf {
namespace {

struct CreateWindowParams {
    Window* instance{};
    const internal::WindowNotCreatedStateData* not_created_state_data{};
};

Point TranslateAbsolutePositionToControlPosition(
    const Point& absolute_position, 
    const Control& control ) {

    auto control_absolute_rect = control.RectInWindow();
    if (!control_absolute_rect) {
        return absolute_position;
    }

    Point result;
    result.x = absolute_position.x - control_absolute_rect->position.x;
    result.y = absolute_position.y - control_absolute_rect->position.y;
    return result;
}

} // namespace

ZAF_OBJECT_IMPL(Window);

LRESULT CALLBACK Window::WindowProcedure(
    HWND hwnd, 
    UINT message_id, 
    WPARAM wparam, 
    LPARAM lparam) {

    if (message_id == WM_NCCREATE) {

        auto create_struct = reinterpret_cast<const CREATESTRUCTA*>(lparam);
        auto create_params =
            reinterpret_cast<const CreateWindowParams*>(create_struct->lpCreateParams);

        auto window = create_params->instance;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<ULONG_PTR>(window));
        window->AttachHandle(hwnd);
    }

    auto window = GetWindowFromHandle(hwnd);
    if (window) {
        return window->RouteWindowMessage(hwnd, message_id, wparam, lparam);
    }

    return CallWindowProc(DefWindowProc, hwnd, message_id, wparam, lparam);
}


Window::Window() : class_(WindowClassRegistry::Instance().GetDefaultWindowClass()) {

}


Window::Window(std::wstring_view window_class_name) {
    class_ = WindowClassRegistry::Instance().GetOrRegisterWindowClass(window_class_name, nullptr);
}


Window::Window(const std::shared_ptr<WindowClass>& window_class) : class_(window_class) {
    ZAF_EXPECT(class_);
}


Window::~Window() {
    // The window handle won't be destroyed in the destructor, as it is destroyed by the 
    // WindowHolder.
}


HWND Window::Handle() const noexcept {
    if (handle_state_ == WindowHandleState::Creating ||
        handle_state_ == WindowHandleState::Created ||
        handle_state_ == WindowHandleState::Destroying) {
        return HandleStateData().handle;
    }
    return nullptr;
}


internal::WindowNotCreatedStateData& Window::NotCreatedStateData() noexcept {
    return std::get<internal::WindowNotCreatedStateData>(state_data_);
}

const internal::WindowNotCreatedStateData& Window::NotCreatedStateData() const noexcept {
    return std::get<internal::WindowNotCreatedStateData>(state_data_);
}


internal::WindowHandleStateData& Window::HandleStateData() noexcept {
    return std::get<internal::WindowHandleStateData>(state_data_);
}

const internal::WindowHandleStateData& Window::HandleStateData() const noexcept {
    return std::get<internal::WindowHandleStateData>(state_data_);
}


void Window::Initialize() {

    __super::Initialize();

    focused_control_manager_ = std::make_unique<internal::WindowFocusedControlManager>(*this);
    Disposables() += focused_control_manager_->FocusedControlChangedEvent().Subscribe(
        [this](const std::shared_ptr<Control>& previous_focused_control) {
            OnFocusedControlChanged(FocusedControlChangedInfo{ 
                shared_from_this(), 
                previous_focused_control 
            });
        });

    SetRootControl(Create<Control>());
}


std::shared_ptr<WindowHolder> Window::CreateHandle() {

    if (handle_state_ == WindowHandleState::NotCreated) {
        return CreateHandleInNotCreatedState();
    }
    else if (handle_state_ == WindowHandleState::Created) {
        return holder_.lock();
    }
    else {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }
}


std::shared_ptr<WindowHolder> Window::CreateHandleInNotCreatedState() {

    // Check if the owner's handle state is valid.
    auto owner = Owner();
    if (owner) {
        auto owner_handle_state = owner->HandleState();
        if (owner_handle_state == WindowHandleState::NotCreated ||
            owner_handle_state == WindowHandleState::Destroying ||
            owner_handle_state == WindowHandleState::Destroyed) {
            throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
        }
    }

    auto holder = std::make_shared<WindowHolder>(shared_from_this());
    holder_ = holder;

    handle_state_ = WindowHandleState::Creating;

    auto not_created_state_data = std::move(NotCreatedStateData());
    state_data_.emplace<internal::WindowHandleStateData>();

    try {

        ProcessCreatingState(not_created_state_data);

        handle_state_ = WindowHandleState::Created;
        ProcessCreatedState();

        return holder;
    }
    catch (...) {

        // If the state is Destroyed, it means Destroy method has been called during the 
        // creation, so don't reset the state to NotCreated.
        if (handle_state_ != WindowHandleState::Destroyed) {

            auto auto_reset = MakeAutoReset(destroy_reason_, DestroyReason::CreationFailed);
            this->Destroy();

            handle_state_ = WindowHandleState::NotCreated;
            state_data_ = std::move(not_created_state_data);
        }

        // The holder member should be reset last, as it is used in the Destroy() method to 
        // detach the window.
        holder_.reset();
        throw;
    }
}


void Window::ProcessCreatingState(const internal::WindowNotCreatedStateData& state_data) {

    auto owner = Owner();

    DWORD style = state_data.basic_style.Value();
    DWORD extended_style = state_data.extended_style.Value();

    auto screen = this->Screen();
    auto initial_rect = GetInitialRect(*screen, owner, state_data);
    auto initial_rect_in_global = screen->TransformToGlobal(initial_rect);

    CreateWindowParams params;
    params.instance = this;
    params.not_created_state_data = &state_data;

    // During the execution of CreateWindowEx, the window handle will be set to handle_ member
    // when the WM_NCCREATE message is received.
    auto handle = CreateWindowEx(
        extended_style,
        reinterpret_cast<LPCWSTR>(class_->GetAtom()),
        state_data.title.c_str(),
        style,
        static_cast<int>(initial_rect_in_global.position.x),
        static_cast<int>(initial_rect_in_global.position.y),
        static_cast<int>(initial_rect_in_global.size.width),
        static_cast<int>(initial_rect_in_global.size.height),
        owner ? owner->Handle() : nullptr,
        nullptr,
        nullptr,
        &params);

    if (!handle) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }
}


void Window::ProcessCreatedState() {

    auto handle = Handle();
    auto dpi = static_cast<float>(GetDpiForWindow(handle));

    RECT client_rect{};
    GetClientRect(handle, &client_rect);
    root_control_->SetRect(ToDIPs(Rect::FromRECT(client_rect), dpi));

    CreateRenderer();

    OnHandleCreated(HandleCreatedInfo{ shared_from_this() });
}


void Window::AttachHandle(HWND handle) noexcept {
    HandleStateData().handle = handle;
}


void Window::OnHandleCreating(const HandleCreatingInfo& event_info) {
    handle_creating_event_.Raise(event_info);
}


rx::Observable<HandleCreatingInfo> Window::HandleCreatingEvent() const {
    return handle_creating_event_.GetObservable();
}


void Window::OnHandleCreated(const HandleCreatedInfo& event_info) {
    handle_created_event_.Raise(event_info);
}


rx::Observable<HandleCreatedInfo> Window::HandleCreatedEvent() const {
    return handle_created_event_.GetObservable();
}


void Window::Destroy() noexcept {

    if (handle_state_ == WindowHandleState::NotCreated) {

        handle_state_ = WindowHandleState::Destroyed;
        OnDestroyed(DestroyedInfo{ shared_from_this(), nullptr, DestroyReason::Unspecified });
    }
    else if (handle_state_ == WindowHandleState::Creating ||
             handle_state_ == WindowHandleState::Created) {

        // Handle related resources would be released in the WM_DESTROY message handler.
        // OnDestroy method would be called in the WM_DESTROY message handler as well.
        DestroyWindow(Handle());
    }
}


void Window::OnDestroying(const DestroyingInfo& event_info) {
    destroying_event_.Raise(event_info);
}


rx::Observable<DestroyingInfo> Window::DestroyingEvent() const {
    return destroying_event_.GetObservable();
}


void Window::OnDestroyed(const DestroyedInfo& event_info) {
    destroyed_event_.Raise(event_info);
}


rx::Observable<DestroyedInfo> Window::DestroyedEvent() const {
    return destroyed_event_.GetObservable();
}


LRESULT Window::HandleWMCREATE(const Message& message) {
    OnHandleCreating(HandleCreatingInfo{ shared_from_this() });
    return 0;
}


zaf::Rect Window::GetInitialRect(
    const zaf::Screen& screen,
    const std::shared_ptr<Window>& owner,
    const internal::WindowNotCreatedStateData& state_data) {

    // The window has a specified initial position, use it.
    if (state_data.initial_position) {
        return zaf::Rect{
            *state_data.initial_position,
            state_data.size
        };
    }

    // The window doesn't have a specified initial position, use the center position of its owner 
    // if there is one.
    if (owner) {
        auto owner_rect = owner->Rect();
        Point position{
            owner_rect.position.x + (owner_rect.size.width - state_data.size.width) / 2,
            owner_rect.position.y + (owner_rect.size.height - state_data.size.height) / 2
        };
        auto result = zaf::Rect{ position, state_data.size };
        return SnapToPixels(result, screen.DPI());
    }

    // Use the center position of the screen.
    auto screen_rect = screen.RectInSelf();
    Point position{
        (screen_rect.size.width - state_data.size.width) / 2,
        (screen_rect.size.height - state_data.size.height) / 2
    };
    auto result = zaf::Rect{ position, state_data.size };
    return SnapToPixels(result, screen.DPI());
}


void Window::CreateRenderer() {

    renderer_ = GraphicFactory::Instance().CreateWindowRenderer(Handle());
}


void Window::RecreateRenderer() {

    root_control_->ReleaseRendererResources();
    CreateRenderer();
}


void Window::Show(ShowOptions options) {

    bool no_activate = 
        HasFlag(options, ShowOptions::NoActivate) || 
        HasFlag(ActivateOptions(), zaf::ActivateOptions::NoAutoActivate);

    InnerShowWindow(no_activate ? SW_SHOWNA : SW_SHOW);
}


void Window::InnerShowWindow(int show_command) {

    std::shared_ptr<WindowHolder> holder;
    if (handle_state_ == WindowHandleState::NotCreated) {
        holder = CreateHandle();
    }
    else if (handle_state_ == WindowHandleState::Creating ||
             handle_state_ == WindowHandleState::Created) {
        holder = holder_.lock();
    }
    else {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }

    Application::Instance().RegisterShownWindow(holder);
    ShowWindow(Handle(), show_command);
}


void Window::Maximize() {
    InnerShowWindow(SW_SHOWMAXIMIZED);
}


bool Window::IsWindowMaximized() const noexcept {
    auto handle = Handle();
    if (handle) {
        return IsMaximized(handle);
    }
    return false;
}


void Window::Minimize() {
    InnerShowWindow(SW_SHOWMINIMIZED);
}


bool Window::IsWindowMinimized() const noexcept {
    auto handle = Handle();
    if (handle) {
        return IsMinimized(handle);
    }
    return false;
}


void Window::Restore() {
    InnerShowWindow(SW_RESTORE);
}


void Window::Hide() noexcept {

    if (handle_state_ == WindowHandleState::Created ||
        handle_state_ == WindowHandleState::Creating) {
        ShowWindow(Handle(), SW_HIDE);
    }
}


std::shared_ptr<Window> Window::Owner() const noexcept {
    return owner_.lock();
}


void Window::SetOwner(std::shared_ptr<Window> owner) {

    if (handle_state_ != WindowHandleState::NotCreated) {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }

    if (this == owner.get()) {
        throw InvalidOperationError(ZAF_SOURCE_LOCATION());
    }

    owner_ = std::move(owner);
}


std::wstring Window::Title() const {

    if (handle_state_ == WindowHandleState::NotCreated) {
        return NotCreatedStateData().title;
    }
    else if (handle_state_ == WindowHandleState::Creating ||
             handle_state_ == WindowHandleState::Created ||
             handle_state_ == WindowHandleState::Destroying) {

        const auto& handle_state = HandleStateData();

        SetLastError(0);
        int title_length = GetWindowTextLength(handle_state.handle);
        if (title_length == 0) {
            auto last_error = GetLastError();
            if (last_error) {
                ZAF_THROW_WIN32_ERROR(last_error);
            }
            return {};
        }

        std::wstring title;
        title.resize(title_length + 1);

        int actual_length = GetWindowText(
            handle_state.handle, 
            title.data(), 
            static_cast<int>(title.size()));

        if (actual_length == 0) {
            ZAF_THROW_WIN32_ERROR(GetLastError());
        }
        title.resize(actual_length);
        return title;
    }
    else {
        return {};
    }
}


void Window::SetTitle(const std::wstring& title) {

    if (handle_state_ == WindowHandleState::NotCreated) {
        NotCreatedStateData().title = title;
    }
    else if (handle_state_ == WindowHandleState::Creating ||
             handle_state_ == WindowHandleState::Created) {

        BOOL is_succeeded = SetWindowText(HandleStateData().handle, title.c_str());
        if (!is_succeeded) {
            ZAF_THROW_WIN32_ERROR(GetLastError());
        }
    }
    else {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }
}


template<typename PROPERTY>
bool Window::HasStyleProperty(PROPERTY property) const noexcept {

    if (handle_state_ == WindowHandleState::NotCreated) {
        return internal::WindowStyleShim<PROPERTY>::Has(NotCreatedStateData(), property);
    }

    if (handle_state_ == WindowHandleState::Creating ||
        handle_state_ == WindowHandleState::Created ||
        handle_state_ == WindowHandleState::Destroying) {

        return internal::WindowStyleShim<PROPERTY>::Has(HandleStateData(), property);
    }

    return false;
}


template<typename PROPERTY>
void Window::SetStyleProperty(PROPERTY property, bool enable, bool can_set_if_has_handle) {

    if (handle_state_ == WindowHandleState::NotCreated) {
        internal::WindowStyleShim<PROPERTY>::Set(NotCreatedStateData(), property, enable);
    }
    else if (handle_state_ == WindowHandleState::Creating ||
             handle_state_ == WindowHandleState::Created) {

        if (!can_set_if_has_handle) {
            throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
        }

        internal::WindowStyleShim<PROPERTY>::Set(HandleStateData(), property, enable);
    }
    else {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }
}


bool Window::IsPopup() const noexcept {
    return HasStyleProperty(internal::WindowBasicStyleProperty::IsPopup);
}

void Window::SetIsPopup(bool is_popup) {
    SetStyleProperty(internal::WindowBasicStyleProperty::IsPopup, is_popup, false);
}


bool Window::HasBorder() const noexcept {
    return HasStyleProperty(internal::WindowBasicStyleProperty::HasBorder);
}

void Window::SetHasBorder(bool has_border) {
    SetStyleProperty(internal::WindowBasicStyleProperty::HasBorder, has_border, false);
}


bool Window::HasTitleBar() const noexcept {
    return HasStyleProperty(internal::WindowBasicStyleProperty::HasTitleBar);
}

void Window::SetHasTitleBar(bool has_title_bar) {
    SetStyleProperty(internal::WindowBasicStyleProperty::HasTitleBar, has_title_bar, false);
}


bool Window::HasSystemMenu() const noexcept {
    return HasStyleProperty(internal::WindowBasicStyleProperty::HasSystemMenu);
}

void Window::SetHasSystemMenu(bool has_system_menu) {
    SetStyleProperty(internal::WindowBasicStyleProperty::HasSystemMenu, has_system_menu, false);
}


bool Window::IsSizable() const noexcept {
    return HasStyleProperty(internal::WindowBasicStyleProperty::IsSizable);
}

void Window::SetIsSizable(bool is_sizable) {
    SetStyleProperty(internal::WindowBasicStyleProperty::IsSizable, is_sizable, true);
}


bool Window::CanMaximize() const noexcept {
    return HasStyleProperty(internal::WindowBasicStyleProperty::CanMaximize);
}

void Window::SetCanMaximize(bool has_maximize_button) {
    SetStyleProperty(internal::WindowBasicStyleProperty::CanMaximize, has_maximize_button, true);
}


bool Window::CanMinimize() const noexcept {
    return HasStyleProperty(internal::WindowBasicStyleProperty::CanMinimize);
}


void Window::SetCanMinimize(bool can_minimize) {
    SetStyleProperty(internal::WindowBasicStyleProperty::CanMinimize, can_minimize, true);
}


bool Window::IsToolWindow() const noexcept {
    return HasStyleProperty(internal::WindowExtendedStyleProperty::IsToolWindow);
}

void Window::SetIsToolWindow(bool is_tool_window) {
    SetStyleProperty(internal::WindowExtendedStyleProperty::IsToolWindow, is_tool_window, true);
}


bool Window::IsTopmost() const noexcept {
    return HasStyleProperty(internal::WindowExtendedStyleProperty::IsTopMost);
}

void Window::SetIsTopmost(bool is_topmost) {

    if (handle_state_ == WindowHandleState::NotCreated) {

        NotCreatedStateData().extended_style.Set(
            internal::WindowExtendedStyleProperty::IsTopMost, 
            is_topmost);
    }
    else if (handle_state_ == WindowHandleState::Creating ||
             handle_state_ == WindowHandleState::Created) {

        BOOL is_succeeded = SetWindowPos(
            HandleStateData().handle,
            is_topmost ? HWND_TOPMOST : HWND_NOTOPMOST,
            0,
            0,
            0,
            0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

        if (!is_succeeded) {
            ZAF_THROW_WIN32_ERROR(GetLastError());
        }
    }
    else {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }
}


ActivateOptions Window::ActivateOptions() const noexcept {
    return activate_options_;
}

void Window::SetActivateOptions(zaf::ActivateOptions options) {

    if (handle_state_ == WindowHandleState::Destroyed) {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }

    SetStyleProperty(
        internal::WindowExtendedStyleProperty::NoActivate, 
        HasFlag(options, ActivateOptions::NoAutoActivate),
        true);

    activate_options_ = options;
}


bool Window::PreprocessMessage(const KeyMessage& message) {

    if (focused_control_manager_->TryToPreprocessTabKeyMessage(message)) {
        return true;
    }

    if (TryToPreprocessInspectorShortcutMessage(message)) {
        return true;
    }

    return false;
}


bool Window::TryToPreprocessInspectorShortcutMessage(const KeyMessage& message) {
#ifndef NDEBUG
    if (message.ID() != WM_KEYDOWN) {
        return false;
    }

    if (message.Key() != Key::F12) {
        return false;
    }

    if ((GetKeyState(VK_CONTROL) >> 15) == 0) {
        return false;
    }

    ShowInspectorWindow();
    return true;
#else
    return false;
#endif
}


LRESULT Window::RouteWindowMessage(HWND hwnd, UINT id, WPARAM wparam, LPARAM lparam) {

    Message message{ hwnd, id, wparam, lparam };

    //Route message received event first.
    MessageReceivedInfo message_received_info{ shared_from_this(), message };
    OnMessageReceived(message_received_info);

    //Check if the message has been handled, pass it to default window procedure if not.
    auto handle_result = message_received_info.HandleResult();
    if (!handle_result) {
        handle_result = CallWindowProc(DefWindowProc, hwnd, id, wparam, lparam);
    }

    //Route message handled event then.
    MessageHandledInfo message_handled_info{ shared_from_this(), message, *handle_result };
    OnMessageHandled(message_handled_info);

    return *handle_result;
}


void Window::OnMessageReceived(const MessageReceivedInfo& event_info) {

    RaiseMessageReceivedEvent(event_info);
    if (event_info.IsHandled()) {
        return;
    }

    auto result = HandleMessage(event_info.Message());
    if (result) {
        event_info.MarkAsHandled(*result);
    }
}


void Window::RaiseMessageReceivedEvent(const MessageReceivedInfo& event_info) {

    if (event_info.Message().ID() != WM_NCDESTROY) {
        message_received_event_.Raise(event_info);
    }
}


rx::Observable<MessageReceivedInfo> Window::MessageReceivedEvent() const {
    return message_received_event_.GetObservable();
}


std::optional<LRESULT> Window::HandleMessage(const Message& message) {

    switch (message.ID()) {
    case WM_CREATE:
        return HandleWMCREATE(message);

    case WM_NCCALCSIZE:
        return HandleWMNCCALCSIZE(message);

    case WM_ERASEBKGND:
        //Don't erase background to avoid blinking.
        return TRUE;
        
    case WM_PAINT:
        HandleWMPAINT();
        return 0;

    case WM_GETMINMAXINFO: {
        auto dpi = this->DPI();
        auto min_max_info = reinterpret_cast<MINMAXINFO*>(message.LParam());
        min_max_info->ptMinTrackSize.x = static_cast<LONG>(FromDIPs(MinWidth(), dpi));
        min_max_info->ptMinTrackSize.y = static_cast<LONG>(FromDIPs(MinHeight(), dpi));
        min_max_info->ptMaxTrackSize.x = static_cast<LONG>(FromDIPs(MaxWidth(), dpi));
        min_max_info->ptMaxTrackSize.y = static_cast<LONG>(FromDIPs(MaxHeight(), dpi));
        return 0;
    }

    case WM_SHOWWINDOW:
        HandleWMSHOWWINDOW(ShowWindowMessage{ message });
        return 0;

    case WM_ACTIVATE:
        HandleWMACTIVATE(ActivateMessage{ message });
        return 0;

    case WM_ENTERSIZEMOVE:
        handle_specific_state_.is_sizing_or_moving = true;
        return 0;

    case WM_EXITSIZEMOVE:
        handle_specific_state_.is_sizing_or_moving = false;
        if (handle_specific_state_.exit_sizing_or_moving_subject) {
            auto observer = handle_specific_state_.exit_sizing_or_moving_subject->AsObserver();
            observer.OnSuccess({});
            handle_specific_state_.exit_sizing_or_moving_subject.reset();
        }
        return 0;

    case WM_SIZE:
        HandleWMSIZEMessage(message);
        return 0;

    case WM_MOVE:
        HandleMoveMessage();
        return 0;

    case WM_SETFOCUS:
        HandleWMSETFOCUS(SetFocusMessage{ message });
        return 0;

    case WM_KILLFOCUS: 
        HandleWMKILLFOCUS(KillFocusMessage{ message });
        return 0;

    case WM_MOUSEACTIVATE: {
        auto activate_option = ActivateOptions();
        bool no_activate = HasFlag(activate_option, ActivateOptions::NoMouseActivate);
        bool discard_message = HasFlag(activate_option, ActivateOptions::DiscardMouseMessage);
        if (no_activate) {
            return discard_message ? MA_NOACTIVATEANDEAT : MA_NOACTIVATE;
        }
        else {
            return discard_message ? MA_ACTIVATEANDEAT : MA_ACTIVATE;
        }
    }

    case WM_CAPTURECHANGED: {
        CancelMouseCapture();
        return 0;
    }

    case WM_NCHITTEST: {
        auto hit_test_result = HitTest(HitTestMessage{ message });
        if (hit_test_result) {
            return static_cast<LRESULT>(*hit_test_result);
        }
        else {
            return std::nullopt;
        }
    }

    case WM_SETCURSOR: {
        if (HandleWMSETCURSOR(message)) {
            return TRUE;
        }
        return std::nullopt;
    }

    case WM_MOUSEWHEEL:
    case WM_MOUSEHWHEEL: {
        //Mouse wheel messages are not sent to an unfocused window even if it captures
        //the mouse, because these messages are only sent to focused window.
        //But we wish these messages have the same behaviour as other mouse input messages,
        //so the messages are redircted to the window which is capturing the mouse.
        if (RedirectMouseWheelMessage(message)) {
            return 0;
        }

        if (HandleMouseMessage(MouseWheelMessage{ message })) {
            return 0;
        }

        return std::nullopt;
    }

    case WM_MOUSEMOVE:
    case WM_NCMOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_NCLBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_NCLBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP: {
        if (HandleMouseMessage(MouseMessage{ message })) {
            return 0;
        }
        return std::nullopt;
    }

    case WM_MOUSEHOVER:
    case WM_NCMOUSEHOVER:
        OnMouseHover(message);
        return 0;

    case WM_MOUSELEAVE:
    case WM_NCMOUSELEAVE:
        OnMouseLeave(message);
        return 0;

    case WM_KEYDOWN: 
    case WM_KEYUP:
    case WM_CHAR:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_SYSCHAR:
        if (HandleKeyboardMessage(message)) {
            return 0;
        }
        return std::nullopt;

    case WM_IME_STARTCOMPOSITION:
    case WM_IME_COMPOSITION:
    case WM_IME_ENDCOMPOSITION:
        HandleIMEMessage(message);
        //For now, we always pass IME messages to the default window procedure even if we handle 
        //the messages. This may be adjusted once we are more familiar with the IME mechanism.
        return std::nullopt;

    case WM_CLOSE:
        HandleWMCLOSE();
        return 0;

    case WM_DESTROY:
        HandleWMDESTROY();
        return 0;

    case WM_NCDESTROY:
        HandleWMNCDESTROY();
        return 0;

    default:
        return std::nullopt;
    }
}


void Window::OnMessageHandled(const MessageHandledInfo& event_info) {
    message_handled_event_.Raise(event_info);
}


rx::Observable<MessageHandledInfo> Window::MessageHandledEvent() const {
    return message_handled_event_.GetObservable();
}


void Window::HandleWMPAINT() {

    auto handle = Handle();

    zaf::Rect dirty_rect;
    RECT win32_rect{};
    if (GetUpdateRect(handle, &win32_rect, TRUE)) {
        dirty_rect = ToDIPs(Rect::FromRECT(win32_rect), DPI());
    }
    else {
        dirty_rect = root_control_->Rect();
    }

    //The update rect must be validated before painting.
    //Because some controls may call NeedRepaint while it is painting,
    //and this may fails if there is an invalidated update rect.
    ValidateRect(handle, nullptr);

    renderer_.BeginDraw();
    Canvas canvas(renderer_);
    {
        auto layer_guard = canvas.PushRegion(root_control_->Rect(), dirty_rect);

        //Paint window background color first.
        {
            auto state_guard = canvas.PushState();
            canvas.SetBrushWithColor(Color::FromRGB(internal::ControlBackgroundColorRGB));
            canvas.DrawRectangle(dirty_rect);
        }

        root_control_->Repaint(canvas, dirty_rect);

        PaintInspectedControl(canvas, dirty_rect);
    }

    try {
        renderer_.EndDraw();
    }
    catch (const COMError& error) {
        if (error.code() == COMError::MakeCode(D2DERR_RECREATE_TARGET)) {
            RecreateRenderer();
        }
    }
}


void Window::PaintInspectedControl(Canvas& canvas, const zaf::Rect& dirty_rect) {

    if (!highlight_control_) {
        return;
    }

    auto control_rect = highlight_control_->RectInWindow();
    if (!control_rect) {
        return;
    }

    if (!control_rect->HasIntersection(dirty_rect)) {
        return;
    }

    auto padding_rect = *control_rect;
    padding_rect.Deflate(highlight_control_->Border());

    auto content_rect = padding_rect;
    content_rect.Deflate(highlight_control_->Padding());

    auto margin_rect = *control_rect;
    margin_rect.Inflate(highlight_control_->Margin());

    auto draw_frame = [&canvas](
        const zaf::Rect& rect,
        const zaf::Rect excluded_rect,
        std::uint32_t color_rgb) {
    
        auto rect_geometry = canvas.CreateRectangleGeometry(rect);
        auto excluded_geometry = canvas.CreateRectangleGeometry(excluded_rect);

        auto frame_geometry = canvas.CreatePathGeometry();
        auto sink = frame_geometry.Open();
        d2d::Geometry::Combine(
            rect_geometry, 
            excluded_geometry, 
            d2d::Geometry::CombineMode::Exclude, 
            sink);
        sink.Close();
        
        auto color = Color::FromRGB(color_rgb);
        color.a /= 2.f;
        canvas.SetBrushWithColor(color);
        canvas.DrawGeometry(frame_geometry);
    };

    auto state_guard = canvas.PushState();
    auto clipping_guard = canvas.PushClipping(dirty_rect);

    //Draw content rect.
    draw_frame(content_rect, zaf::Rect{}, internal::InspectedControlContentColor);

    //Draw padding rect.
    draw_frame(padding_rect, content_rect, internal::InspectedControlPaddingColor);

    //Draw border rect.
    draw_frame(*control_rect, padding_rect, internal::InspectedControlBorderColor);

    //Draw margin rect.
    draw_frame(margin_rect, *control_rect, internal::InspectedControlMarginColor);
}


void Window::NeedRepaintRect(const zaf::Rect& rect) {

    auto handle = Handle();
    if (handle != nullptr) {
        RECT win32_rect = SnapAndTransformToPixels(rect, DPI()).ToRECT();
        InvalidateRect(handle, &win32_rect, FALSE);
    }
}


void Window::HandleWMSHOWWINDOW(const ShowWindowMessage& message) {

    internal::ShowWindowEventInfo event_info{ shared_from_this(), message };

    if (message.IsHidden()) {
        OnHide(event_info);
    }
    else {
        OnShow(event_info);
    }
}


void Window::OnShow(const ShowInfo& event_info) {
    show_event_.Raise(event_info);
}


rx::Observable<ShowInfo> Window::ShowEvent() const {
    return show_event_.GetObservable();
}


void Window::OnHide(const HideInfo& event_info) {
    hide_event_.Raise(event_info);
}


rx::Observable<HideInfo> Window::HideEvent() const {
    return hide_event_.GetObservable();
}


void Window::HandleWMACTIVATE(const ActivateMessage& message) {

    internal::ActivateEventInfo event_info{ shared_from_this(), message };

    if (event_info.Message().ActivateState() != ActivateState::Inactive) {
        OnActivated(event_info);
    }
    else {
        OnDeactivated(event_info);
    }
}


void Window::OnActivated(const ActivatedInfo& event_info) {
    activated_event_.Raise(event_info);
}


rx::Observable<ActivatedInfo> Window::ActivatedEvent() const {
    return activated_event_.GetObservable();
}


void Window::OnDeactivated(const DeactivatedInfo& event_info) {
    deactivated_event_.Raise(event_info);
}


rx::Observable<DeactivatedInfo> Window::DeactivatedEvent() const {
    return deactivated_event_.GetObservable();
}


void Window::HandleWMSETFOCUS(const SetFocusMessage& message) {

    focused_control_manager_->HandleWindowSetFocus();
    OnFocusGained(WindowFocusGainedInfo{ shared_from_this(), message.Inner() });
}


void Window::OnFocusGained(const WindowFocusGainedInfo& event_info) {
    focus_gained_event_.Raise(event_info);
}


rx::Observable<WindowFocusGainedInfo> Window::FocusGainedEvent() const {
    return focus_gained_event_.GetObservable();
}


void Window::HandleWMKILLFOCUS(const KillFocusMessage& message) {

    focused_control_manager_->HandleWindowKillFocus();
    OnFocusLost(WindowFocusLostInfo{ shared_from_this(), message.Inner() });
}


void Window::OnFocusLost(const WindowFocusLostInfo& event_info) {
    focus_lost_event_.Raise(event_info);
}


rx::Observable<WindowFocusLostInfo> Window::FocusLostEvent() const {
    return focus_lost_event_.GetObservable();
}


void Window::HandleWMSIZEMessage(const Message& message) {

    //Don't handle size message during window creation procedure,
    //because the object state is inconsistent until creation completed.
    if (!Handle()) {
        return;
    }

    zaf::Size size{
        static_cast<float>(LOWORD(message.LParam())),
        static_cast<float>(HIWORD(message.LParam()))
    };

    if (renderer_) {
        renderer_.Resize(size);
    }

    zaf::Rect root_control_rect{ Point(), ToDIPs(size, DPI()) };
    root_control_->SetRect(root_control_rect);

    OnSizeChanged(WindowSizeChangedInfo{ shared_from_this() });
}


void Window::OnSizeChanged(const WindowSizeChangedInfo& event_info) {
    size_changed_event_.Raise(event_info);
}


rx::Observable<WindowSizeChangedInfo> Window::SizeChangedEvent() const {
    return size_changed_event_.GetObservable();
}


rx::Single<None> Window::WhenNotSizingOrMoving() const {

    if (!handle_specific_state_.is_sizing_or_moving) {
        return rx::Single<None>::Just({});
    }

    if (!handle_specific_state_.exit_sizing_or_moving_subject) {
        handle_specific_state_.exit_sizing_or_moving_subject.emplace();
    }

    return handle_specific_state_.exit_sizing_or_moving_subject->AsSingle();
}


void Window::HandleMoveMessage() {

}


std::optional<LRESULT> Window::HandleWMNCCALCSIZE(const Message& message) {

    //WM_NCCALCSIZE must be passed to default window procedure if wparam is FALSE, no matter if
    //the window has customized style, otherwise the window could have some odd behaviors.
    if (message.WParam() == FALSE) {
        return std::nullopt;
    }

    //We need to remove the default window frame in WM_NCCALCSIZE for overlapped window without 
    //boder. It is no need to do that for popup window without boder.
    bool has_customized_style = !IsPopup() && !HasBorder();
    if (!has_customized_style) {
        return std::nullopt;
    }

    //Return TRUE to remove the default window frame.
    return TRUE;
}


std::optional<HitTestResult> Window::HitTest(const HitTestMessage& message) {

    auto mouse_over_control = [&]() {
        
        Point mouse_position = message.MousePosition();
        std::shared_ptr<Control> current_control = root_control_;
        while (true) {

            auto child = current_control->FindChildAtPosition(mouse_position);
            if (child == nullptr) {
                break;
            }

            auto child_position = child->Position();
            mouse_position.x += child_position.x;
            mouse_position.y += child_position.y;

            current_control = child;
        }
        return current_control;
    }();

    return mouse_over_control->HitTest(message);
}


bool Window::RedirectMouseWheelMessage(const Message& message) {

    HWND handle = GetCapture();
    if ((handle == nullptr) || (handle == Handle())) {
        return false;
    }

    auto capturing_mouse_window = GetWindowFromHandle(GetCapture());
    if (capturing_mouse_window == nullptr) {
        return false;
    }

    PostMessage(
        capturing_mouse_window->Handle(), 
        message.ID(),
        message.WParam(),
        message.LParam());

    return true;
}


bool Window::HandleMouseMessage(const MouseMessage& message) {

    if (message.ID() == WM_MOUSEMOVE || message.ID() == WM_NCMOUSEMOVE) {

        if (is_selecting_inspector_control_) {
            HighlightControlAtPosition(message.MousePosition());
            return true;
        }

        TrackMouseByMouseMove(message);
    }
    else {

        HideTooltipWindow();

        if (message.ID() == WM_LBUTTONDOWN || message.ID() == WM_RBUTTONDOWN) {

            if (is_selecting_inspector_control_) {
                SelectInspectedControl();
                return true;
            }
        }
    }

    return RouteMouseEvent(message);
}


bool Window::RouteMouseEvent(const MouseMessage& message) {

    const auto is_mouse_move = [&message]() {
        return message.ID() == WM_MOUSEMOVE || message.ID() == WM_NCMOUSEMOVE;
    };

    Point position_at_begin_routing_control;
    auto begin_routing_control = GetBeginRoutingControlForMouseMessage(
        message,
        position_at_begin_routing_control);

    auto event_target_info = internal::FindMouseEventTarget(
        begin_routing_control,
        position_at_begin_routing_control);

    if (!event_target_info.control) {
        //Need to clear mouse over control if event target is not found for mouse move message.
        if (is_mouse_move()) {
            SetMouseOverControl(nullptr, MouseMessage{ Message{} });
        }
        return false;
    }

    auto event_info_state = std::make_shared<internal::MouseEventSharedState>(
        event_target_info.control,
        message.Inner(),
        event_target_info.position
    );

    //Tunnel the event.
    internal::TunnelMouseEvent(event_target_info.control, event_info_state, message);
    if (!event_info_state->IsHandled()) {
        //Change mouse over control if the event is not handled in tunneling phase.
        if (is_mouse_move()) {
            SetMouseOverControl(event_target_info.control, message);
        }
    }

    //Bubble the event.
    internal::BubbleMouseEvent(
        event_target_info.control, 
        event_target_info.position,
        event_info_state,
        message);

    return event_info_state->IsHandled();
}


std::shared_ptr<Control> Window::GetBeginRoutingControlForMouseMessage(
    const MouseMessage& message,
    Point& position_at_control) const {

    std::shared_ptr<Control> result;

    if (mouse_capture_control_) {

        result = mouse_capture_control_;

        position_at_control = TranslateAbsolutePositionToControlPosition(
            message.MousePosition(),
            *result);
    }
    else {

        result = root_control_;
        position_at_control = message.MousePosition();
    }

    return result;
}


void Window::HighlightControlAtPosition(const Point& position) {

    auto highlight_control = root_control_->FindChildAtPositionRecursively(position);
    if (!highlight_control) {
        highlight_control = root_control_;
    }

    SetHighlightControl(highlight_control);

    auto inspector_port = GetInspectorPort();
    if (inspector_port) {
        inspector_port->HighlightControl(highlight_control);
    }
}


void Window::SelectInspectedControl() {

    is_selecting_inspector_control_ = false;

    if (!highlight_control_) {
        return;
    }

    auto inspector_port = GetInspectorPort();
    if (inspector_port) {
        inspector_port->SelectControl(highlight_control_); 
    }
    
    SetHighlightControl(nullptr);
}


void Window::TrackMouseByMouseMove(const MouseMessage& message) {

    bool is_non_client = message.ID() == WM_NCMOUSEMOVE;

    auto is_tracking_mouse = [this, is_non_client]() {

        if (track_mouse_mode_ == TrackMouseMode::ClientArea &&
            !is_non_client) {
            return true;
        }

        if (track_mouse_mode_ == TrackMouseMode::NonClientArea &&
            is_non_client) {
            return true;
        }

        return false;
    }();

    if (is_tracking_mouse) {
        return;
    }

    TrackMouse(is_non_client);
}


void Window::TrackMouse(bool is_non_client) {

    //Don't track mouse if the mouse is being captured and the capturing window is not the current 
    //window. This would happen if WM_MOUSEMOVE message is redirected from a capturing window to 
    //another window (this is the way how PopupMenu works).
    auto capturing_window = GetCapture();
    if (capturing_window && capturing_window != Handle()) {
        return;
    }

    TRACKMOUSEEVENT track_mouse_event_param {};
    track_mouse_event_param.cbSize = sizeof(track_mouse_event_param);
    track_mouse_event_param.dwFlags = TME_LEAVE | TME_HOVER;
    if (is_non_client) {
        track_mouse_event_param.dwFlags |= TME_NONCLIENT;
    }
    track_mouse_event_param.hwndTrack = Handle();

    if (TrackMouseEvent(&track_mouse_event_param)) {

        track_mouse_mode_ =
            is_non_client ?
            TrackMouseMode::NonClientArea :
            TrackMouseMode::ClientArea;
    }
}


void Window::OnMouseHover(const Message& message) {

    if (mouse_over_control_) {

        //Raise and route mouse hover event.
        auto event_info_state = std::make_shared<RoutedEventSharedState>(mouse_over_control_);
        for (auto control = mouse_over_control_; control; control = control->Parent()) {
            control->OnMouseHover(MouseHoverInfo{ event_info_state, control });
        }
    }

    TryToShowTooltipWindow();
}


void Window::OnMouseLeave(const Message& message) {

    bool is_tracking_mouse = [&]() {
    
        if (track_mouse_mode_ == TrackMouseMode::ClientArea &&
            message.ID() == WM_MOUSELEAVE) {
            return true;
        }

        if (track_mouse_mode_ == TrackMouseMode::NonClientArea &&
            message.ID() == WM_NCMOUSELEAVE) {
            return true;
        }

        return false;
    }();

    if (is_tracking_mouse) {
        track_mouse_mode_ = TrackMouseMode::None;
        SetMouseOverControl(nullptr, MouseMessage{ Message{} });
    }
}


bool Window::HandleWMSETCURSOR(const Message& message) {

    if (!mouse_over_control_) {
        return false;
    }

    auto event_state = std::make_shared<internal::MouseCursorChangingState>(
        mouse_over_control_,
        message);

    for (auto sender = mouse_over_control_; sender; sender = sender->Parent()) {
        sender->OnMouseCursorChanging(MouseCursorChangingInfo{ event_state, sender });
    }

    return event_state->IsHandled();
}


bool Window::HandleKeyboardMessage(const Message& message) {

    const auto& focused_control = focused_control_manager_->FocusedControl();
    if (focused_control) {
        return internal::RouteKeyboardEvent(focused_control, message);
    }
    return false;
}


void Window::TryToShowTooltipWindow() {

    if (!mouse_over_control_) {
        return;
    }

    auto tooltip = mouse_over_control_->Tooltip();
    if (tooltip.empty()) {
        return;
    }

    if (!tooltip_window_) {
        tooltip_window_ = zaf::Create<zaf::TooltipWindow>();
        tooltip_window_->SetOwner(shared_from_this());
    }

    tooltip_window_->SetText(tooltip);
    tooltip_window_->ShowBelowMouseCursor();
}


void Window::HideTooltipWindow() {
    if (tooltip_window_) {
        tooltip_window_->Hide();
    }
}


void Window::HandleIMEMessage(const Message& message) {

    auto focused_control = this->FocusedControl();
    if (!focused_control) {
        return;
    }

    switch (message.ID()) {
    case WM_IME_STARTCOMPOSITION:
        focused_control->OnIMEStartComposition(IMEStartCompositionInfo{ 
            focused_control,
            message 
        });
        break;
    case WM_IME_ENDCOMPOSITION:
        focused_control->OnIMEEndComposition(IMEEndCompositionInfo{ focused_control, message });
        break;
    case WM_IME_COMPOSITION:
        focused_control->OnIMEComposition(IMECompositionInfo{ focused_control, message });
        break;
    default:
        break;
    }
}


void Window::HandleWMCLOSE() {

    ClosingInfo event_info{ shared_from_this() };
    OnClosing(event_info);

    if (event_info.CanClose()) {
        auto auto_reset = MakeAutoReset(destroy_reason_, DestroyReason::Closed);
        Destroy();
    }
}


void Window::OnClosing(const ClosingInfo& event_info) {
    closing_event_.Raise(event_info);
}


rx::Observable<ClosingInfo> Window::ClosingEvent() const {
    return closing_event_.GetObservable();
}


void Window::HandleWMDESTROY() {

    //Avoid reentering.
    if (handle_state_ == WindowHandleState::Destroying) {
        return;
    }

    handle_state_ = WindowHandleState::Destroying;
    OnDestroying(DestroyingInfo{ shared_from_this(), destroy_reason_ });

    CancelMouseCapture();
    focused_control_manager_->HandleWindowDestroy();
    root_control_->ReleaseRendererResources();

    HWND old_handle = Handle();
    handle_specific_state_ = {};
    renderer_ = {};
    tooltip_window_.reset();

    state_data_.emplace<std::monostate>();
    handle_state_ = WindowHandleState::Destroyed;
    OnDestroyed(DestroyedInfo{ shared_from_this(), old_handle, destroy_reason_ });
}


void Window::HandleWMNCDESTROY() noexcept {

    auto holder = holder_.lock();
    if (holder) {

        Application::Instance().UnregisterShownWindow(holder);

        holder->Detach();
        holder_.reset();
    }
}


void Window::SetMouseOverControl(
    const std::shared_ptr<Control>& mouse_over_control, 
    const MouseMessage& message) {

    if (mouse_over_control_ == mouse_over_control) {
        return;
    }

    if (mouse_over_control != nullptr) {

        //The mouse over control must be contained in this window
        if (mouse_over_control->Window().get() != this) {
            return;
        }
    }

    auto old_control = mouse_over_control_;
    mouse_over_control_ = mouse_over_control;

    if (old_control ) {
        ChangeControlMouseOverState(old_control, false, mouse_over_control_);
    }

    if (mouse_over_control_ != nullptr) {

        //Window finds the mouse over control when received WM_MOUSEMOVE,
        //but WM_SETCURSOR is always received before WM_MOUSEMOVE, in such
        //case the mouse over control cannot change the cursor promptly. So 
        //here, a simulated WM_SETCURSOR is posted to give a change to 
        //change the cursor immediately.
        this->Messager().PostWMSETCURSOR(message);

        ChangeControlMouseOverState(mouse_over_control_, true, old_control);

        //Track mouse again to generate next mouse hover message.
        //TODO: Find out whether the new control is in non-client area.
        TrackMouse(false);
    }

    HideTooltipWindow();
}


void Window::ChangeControlMouseOverState(
    const std::shared_ptr<Control>& target_control,
    bool is_mouse_over,
    const std::shared_ptr<Control>& changed_control) {

    target_control->SetIsMouseOverByWindow(is_mouse_over);

    //Raise and route event.
    auto event_info_state = std::make_shared<internal::MouseOverEventSharedState>(
        target_control, 
        changed_control);

    for (auto sender = target_control; sender; sender = sender->Parent()) {

        //Parents at the route path should update their styles to reflect the change of mouse 
        //over state. For example, parents may use ContainsMouse to update their styles.
        sender->NeedUpdateStyle();

        if (is_mouse_over) {
            sender->OnMouseEnter(MouseEnterInfo{ event_info_state, sender });
        }
        else {
            sender->OnMouseLeave(MouseLeaveInfo{ event_info_state, sender });
        }
    }
}


void Window::SetMouseCaptureControl(
    const std::shared_ptr<Control>& capture_control,
    bool is_releasing) {

    if (capture_control == nullptr) {
        return;
    }

    if (!is_releasing) {
        CaptureMouseWithControl(capture_control);
    }
    else {
        ReleaseMouseWithControl(capture_control);
    }
}


void Window::CaptureMouseWithControl(const std::shared_ptr<Control>& control) {
    
    auto previous_control = mouse_capture_control_;
    if (previous_control == control) {
        return;
    }

    //The control must be contained in this window.
    if (control->Window().get() != this) {
        return;
    }

    if (previous_control != nullptr) {
        previous_control->IsCapturingMouseChanged(false);
    }
    else {
        SetCapture(Handle());
    }

    mouse_capture_control_ = control;
    mouse_capture_control_->IsCapturingMouseChanged(true);

    OnMouseCaptureControlChanged(MouseCaptureControlChangedInfo{ 
        shared_from_this(),
        previous_control 
    });
}


void Window::ReleaseMouseWithControl(const std::shared_ptr<Control>& control) {

    if (mouse_capture_control_ != control) {
        return;
    }

    //ReleaseCapture sends a WM_CAPTURECAHNGED message to the window, 
    //in which message handler will set mouse_capture_control_ to nullptr.
    ReleaseCapture();
}


void Window::CancelMouseCapture() {

    if (!mouse_capture_control_) {
        return;
    }

    //Set mouse_capture_control_ to nullptr before raising events to avoid reentering issues.
    auto previous_control = mouse_capture_control_;
    mouse_capture_control_ = nullptr;
    previous_control->IsCapturingMouseChanged(false);

    OnMouseCaptureControlChanged(MouseCaptureControlChangedInfo{
        shared_from_this(),
        previous_control
    });
}


void Window::OnMouseCaptureControlChanged(const MouseCaptureControlChangedInfo& event_info) {
    mouse_capture_control_changed_event_.Raise(event_info);
}


rx::Observable<MouseCaptureControlChangedInfo> Window::MouseCaptureControlChangedEvent() const {
    return mouse_capture_control_changed_event_.GetObservable();
}


std::shared_ptr<Control> Window::FocusedControl() const {
    return focused_control_manager_->FocusedControl();
}


rx::Observable<FocusedControlChangedInfo> Window::FocusedControlChangedEvent() const {
    return focused_control_changed_event_.GetObservable();
}


void Window::OnFocusedControlChanged(const FocusedControlChangedInfo& event_info) {
    focused_control_changed_event_.Raise(event_info);
}


std::shared_ptr<zaf::Screen> Window::Screen() const noexcept {

    if (screen_) {
        return screen_;
    }

    auto owner = this->Owner();
    if (owner) {
        return owner->Screen();
    }

    return ScreenManager::Instance().PrimaryScreen();
}


void Window::SetScreen(std::shared_ptr<zaf::Screen> screen) {

    ZAF_EXPECT(screen);

    if (handle_state_ != WindowHandleState::NotCreated) {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }

    screen_ = screen;
}


float Window::DPI() const noexcept {
    if (handle_state_ == WindowHandleState::Creating ||
        handle_state_ == WindowHandleState::Created ||
        handle_state_ == WindowHandleState::Destroying) {
        return static_cast<float>(GetDpiForWindow(Handle()));
    }
    return this->Screen()->DPI();
}


Rect Window::Rect() const noexcept {

    if (handle_state_ == WindowHandleState::NotCreated) {
        return GetInitialRect(*this->Screen(), this->Owner(), NotCreatedStateData());
    }

    if (handle_state_ == WindowHandleState::Creating || 
        handle_state_ == WindowHandleState::Created ||
        handle_state_ == WindowHandleState::Destroying) {

        RECT rect{};
        GetWindowRect(Handle(), &rect);
        return ToDIPs(Rect::FromRECT(rect), DPI());
    }

    return {};
}


void Window::SetRect(const zaf::Rect& rect) {

    if (handle_state_ == WindowHandleState::NotCreated) {

        zaf::Rect new_rect{ rect.position, ClampSize(rect.size) };
        new_rect = SnapToPixels(new_rect, DPI());

        auto& not_created_state_data = NotCreatedStateData();
        not_created_state_data.initial_position = new_rect.position;
        not_created_state_data.size = new_rect.size;
    }
    else if (handle_state_ == WindowHandleState::Creating || 
             handle_state_ == WindowHandleState::Created) {

        auto new_rect = rect;
        new_rect.size = ClampSize(new_rect.size);
        new_rect = SnapToPixels(new_rect, DPI());
        auto new_rect_in_global = this->Screen()->TransformToGlobal(new_rect);
        
        BOOL is_succeeded = SetWindowPos(
            Handle(),
            nullptr,
            static_cast<int>(new_rect_in_global.position.x),
            static_cast<int>(new_rect_in_global.position.y),
            static_cast<int>(new_rect_in_global.size.width),
            static_cast<int>(new_rect_in_global.size.height),
            SWP_NOZORDER | SWP_NOACTIVATE);

        if (!is_succeeded) {
            ZAF_THROW_WIN32_ERROR(GetLastError());
        }
    }
    else {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }
}


zaf::Size Window::ClampSize(const zaf::Size& size) const noexcept {
    zaf::Size result;
    result.width = std::clamp(size.width, MinWidth(), MaxWidth());
    result.height = std::clamp(size.height, MinHeight(), MaxHeight());
    return result;
}


Point Window::Position() const noexcept {
    return Rect().position;
}

void Window::SetPosition(const Point& position) {
    zaf::Rect new_rect = Rect();
    new_rect.position = position;
    SetRect(new_rect);
}


zaf::Size Window::Size() const noexcept {
    return Rect().size;
}

void Window::SetSize(const zaf::Size& size) {
    if (handle_state_ == WindowHandleState::NotCreated) {
        NotCreatedStateData().size = ClampSize(size);
    }
    else {
        zaf::Rect new_rect = Rect();
        new_rect.size = size;
        SetRect(new_rect);
    }
}


float Window::Width() const noexcept {
    return Size().width;
}

void Window::SetWidth(float width) {
    zaf::Size new_size = Size();
    new_size.width = width;
    SetSize(new_size);
}


float Window::Height() const noexcept {
    return Size().height;
}

void Window::SetHeight(float height) {
    zaf::Size new_size = Size();
    new_size.height = height;
    SetSize(new_size);
}


float Window::MinWidth() const noexcept {
    if (min_width_) {
        return *min_width_;
    }
    return ToDIPs(static_cast<float>(GetSystemMetrics(SM_CXMINTRACK)), DPI());
}

void Window::SetMinWidth(float min_width) {

    if (handle_state_ == WindowHandleState::Destroying ||
        handle_state_ == WindowHandleState::Destroyed) {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }

    auto backup_min_width = min_width_;
    auto backup_max_width = max_width_;

    min_width_ = min_width;

    if (MaxWidth() < min_width) {
        max_width_ = min_width;
    }

    try {
        if (Width() < min_width) {
            SetWidth(min_width);
        }
    }
    catch (...) {
        min_width_ = backup_min_width;
        max_width_ = backup_max_width;
        throw;
    }
}


float Window::MaxWidth() const noexcept {
    if (max_width_) {
        return *max_width_;
    }
    return ToDIPs(static_cast<float>(GetSystemMetrics(SM_CXMAXTRACK)), DPI());
}

void Window::SetMaxWidth(float max_width) {

    if (handle_state_ == WindowHandleState::Destroying ||
        handle_state_ == WindowHandleState::Destroyed) {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }

    auto backup_min_width = min_width_;
    auto backup_max_width = max_width_;

    max_width_ = max_width;

    if (MinWidth() > max_width) {
        min_width_ = max_width;
    }

    try {
        if (Width() > max_width) {
            SetWidth(max_width);
        }
    }
    catch (...) {
        min_width_ = backup_min_width;
        max_width_ = backup_max_width;
        throw;
    }
}


float Window::MinHeight() const noexcept {
    if (min_height_) {
        return *min_height_;
    }
    return ToDIPs(static_cast<float>(GetSystemMetrics(SM_CYMINTRACK)), DPI());
}

void Window::SetMinHeight(float min_height) {

    if (handle_state_ == WindowHandleState::Destroying ||
        handle_state_ == WindowHandleState::Destroyed) {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }

    auto backup_min_height = min_height_;
    auto backup_max_height = max_height_;

    min_height_ = min_height;

    if (MaxHeight() < min_height) {
        max_height_ = min_height;
    }

    try {
        if (Height() < min_height) {
            SetHeight(min_height);
        }
    }
    catch (...) {
        min_height_ = backup_min_height;
        max_height_ = backup_max_height;
        throw;
    }
}


float Window::MaxHeight() const noexcept {
    if (max_height_) {
        return *max_height_;
    }
    return ToDIPs(static_cast<float>(GetSystemMetrics(SM_CYMAXTRACK)), DPI());
}

void Window::SetMaxHeight(float max_height) {

    if (handle_state_ == WindowHandleState::Destroying ||
        handle_state_ == WindowHandleState::Destroyed) {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }

    auto backup_min_height = min_height_;
    auto backup_max_height = max_height_;

    max_height_ = max_height;

    if (MinHeight() > max_height) {
        min_height_ = max_height;
    }

    try {
        if (Height() > max_height) {
            SetHeight(max_height);
        }
    }
    catch (...) {
        min_height_ = backup_min_height;
        max_height_ = backup_max_height;
        throw;
    }
}


zaf::Size Window::MinSize() const noexcept {
    return zaf::Size{ MinWidth(), MinHeight() };
}

void Window::SetMinSize(const zaf::Size& size) {
    SetMinWidth(size.width);
    SetMinHeight(size.height);
}


zaf::Size Window::MaxSize() const noexcept {
    return zaf::Size{ MaxWidth(), MaxHeight() };
}

void Window::SetMaxSize(const zaf::Size& size) {
    SetMaxWidth(size.width);
    SetMaxHeight(size.height);
}


zaf::Rect Window::ContentRect() const noexcept {
    return zaf::Rect{ Point{}, ContentSize() };
}


zaf::Size Window::ContentSize() const noexcept {

    if (handle_state_ == WindowHandleState::NotCreated) {

        auto& not_created_state_data = NotCreatedStateData();
        auto adjusted_size = AdjustContentSizeToWindowSize(
            zaf::Size{}, 
            not_created_state_data.basic_style,
            not_created_state_data.extended_style);
        
        auto result = not_created_state_data.size;
        result.width = (std::max)(result.width - adjusted_size.width, 0.f);
        result.height = (std::max)(result.height - adjusted_size.height, 0.f);
        return result;
    }

    if (handle_state_ == WindowHandleState::Creating ||
        handle_state_ == WindowHandleState::Created ||
        handle_state_ == WindowHandleState::Destroying) {

        RECT client_rect{};
        ::GetClientRect(Handle(), &client_rect);
        return ToDIPs(Rect::FromRECT(client_rect).size, DPI());
    }

    return {};
}

void Window::SetContentSize(const zaf::Size& size) {

    zaf::Size window_size;
    if (handle_state_ == WindowHandleState::NotCreated) {

        auto& not_created_state_data = NotCreatedStateData();
        window_size = AdjustContentSizeToWindowSize(
            size,
            not_created_state_data.basic_style,
            not_created_state_data.extended_style);
    }
    else if (handle_state_ == WindowHandleState::Creating ||
             handle_state_ == WindowHandleState::Created) {

        window_size = AdjustContentSizeToWindowSize(
            size,
            internal::WindowBasicStyle::FromHandle(Handle()),
            internal::WindowExtendedStyle::FromHandle(Handle()));
    }
    else {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }
    SetSize(window_size);
}


zaf::Size Window::AdjustContentSizeToWindowSize(
    const zaf::Size& content_size,
    const internal::WindowBasicStyle& basic_style,
    const internal::WindowExtendedStyle& extend_style) const noexcept {

    auto dpi = DPI();

    zaf::Rect rect{ zaf::Point{}, content_size };
    auto snapped_rect = SnapAndTransformToPixels(rect, dpi);
    auto adjusted_rect = snapped_rect.ToRECT();

    AdjustWindowRectExForDpi(
        &adjusted_rect, 
        basic_style.Value(),
        FALSE, 
        extend_style.Value(),
        static_cast<UINT>(dpi));

    auto result = zaf::Size{
        static_cast<float>(adjusted_rect.right - adjusted_rect.left),
        static_cast<float>(adjusted_rect.bottom - adjusted_rect.top)
    };

    result = ToDIPs(result, dpi);
    return result;
}


float Window::ContentWidth() const noexcept {
    return ContentSize().width;
}

void Window::SetContentWidth(float width) {
    zaf::Size new_size = ContentSize();
    new_size.width = width;
    SetContentSize(new_size);
}


float Window::ContentHeight() const noexcept {
    return ContentSize().height;
}

void Window::SetContentHeight(float height) {
    zaf::Size new_size = ContentSize();
    new_size.height = height;
    SetContentSize(new_size);
}


void Window::SetRootControl(const std::shared_ptr<Control>& control) {

    ZAF_EXPECT(control);
    ZAF_EXPECT(!control->Parent());

    //The same root control is allowed to be set multiple times.
    if (root_control_ == control) {
        return;
    }

    ZAF_EXPECT(!control->Window());

    //Cancel the focused control before changing the root control.
    auto focused_control = FocusedControl();
    if (focused_control) {
        focused_control->SetIsFocused(false);
    }

    auto previous_root_control = root_control_;
    if (previous_root_control) {

        previous_root_control->ReleaseRendererResources();
        previous_root_control->SetWindow(nullptr);
        previous_root_control->RaiseWindowChangedEvent(shared_from_this());
    }

    root_control_ = control;

    auto old_window = root_control_->Window();
    root_control_->SetWindow(shared_from_this());
    root_control_->RaiseWindowChangedEvent(old_window);

    if (auto handle = Handle()) {
        RECT client_rect{};
        ::GetClientRect(handle, &client_rect);
        root_control_->SetRect(Rect::FromRECT(client_rect));
    }

    OnRootControlChanged(RootControlChangedInfo{ shared_from_this(), previous_root_control });
}


void Window::OnRootControlChanged(const RootControlChangedInfo& event_info) {
    root_control_changed_event_.Raise(event_info);
}


rx::Observable<RootControlChangedInfo> Window::RootControlChangedEvent() const {
    return root_control_changed_event_.GetObservable();
}


Point Window::GetMousePosition() const {

    POINT cursor_point = { 0 };
    GetCursorPos(&cursor_point);
    ScreenToClient(Handle(), &cursor_point);

    Point point_in_pixels{ 
        static_cast<float>(cursor_point.x), 
        static_cast<float>(cursor_point.y) 
    };

    return ToDIPs(point_in_pixels, DPI());
}


Point Window::TranslateToScreen(const Point& position) const {

    auto handle = Handle();
    ZAF_EXPECT(handle);

    auto dpi = DPI();
    auto position_in_pixel = FromDIPs(position, dpi);

    auto point = position_in_pixel.ToPOINT();
    ClientToScreen(handle, &point);

    auto result = Point::FromPOINT(point);
    return ToDIPs(result, dpi);
}


Point Window::TranslateFromScreen(const Point& position_in_screen) const {

    auto handle = Handle();
    ZAF_EXPECT(handle);

    auto dpi = DPI();
    auto position_in_pixel = FromDIPs(position_in_screen, dpi);

    auto point = position_in_pixel.ToPOINT();
    ScreenToClient(handle, &point);

    auto result = Point::FromPOINT(point);
    return ToDIPs(result, dpi);
}


bool Window::Activate() {
    ZAF_EXPECT(Handle());
    return !!SetForegroundWindow(Handle());
}


bool Window::IsVisible() const noexcept {
    return !!IsWindowVisible(Handle());
}


bool Window::IsFocused() const {

    auto handle = Handle();
    if (!handle) {
        return false;
    }

    return GetFocus() == handle;
}


void Window::Close() {

    //Do nothing if the window is being destroyed.
    if (handle_state_ == WindowHandleState::Destroying) {
        return;
    }

    SendMessage(Handle(), WM_CLOSE, 0, 0);
}


WindowMessager Window::Messager() {
    ZAF_EXPECT(Handle());
    return WindowMessager{ Handle() };
}


void Window::ShowInspectorWindow() {

    if (inspector_window_.lock()) {
        return;
    }

    auto inspector_window = Create<InspectorWindow>(shared_from_this());
    inspector_window->Show();

    inspector_window_ = inspector_window;
}


void Window::SetHighlightControl(const std::shared_ptr<Control>& highlight_control) {

    if (highlight_control_ == highlight_control) {
        return;
    }

    if (!highlight_control) {
        highlight_control_ = nullptr;
        NeedRepaintRect(root_control_->Rect());
        return;
    }

    if (highlight_control->Window().get() != this) {
        return;
    }

    //Repaint the rect of previous highlight control.
    if (highlight_control_) {
        auto rect_in_window = highlight_control_->RectInWindow();
        if (rect_in_window) {
            NeedRepaintRect(*rect_in_window);
        }
    }

    highlight_control_ = highlight_control;

    //Repaint the rect of new highlight control.
    NeedRepaintRect(*highlight_control_->RectInWindow());
}


std::shared_ptr<internal::InspectorPort> Window::GetInspectorPort() const {

    auto inspector_window = inspector_window_.lock();
    if (!inspector_window) {
        return nullptr;
    }

    return inspector_window->GetPort();
}


void Window::BeginSelectInspectedControl() {

    is_selecting_inspector_control_ = true;
}


std::shared_ptr<Window> GetWindowFromHandle(HWND handle) {

    if (handle == nullptr) {
        return nullptr;
    }

    LONG_PTR user_data = GetWindowLongPtr(handle, GWLP_USERDATA);
    Window* window = reinterpret_cast<Window*>(user_data);
    if (window == nullptr) {
        return nullptr;
    }

    return window->shared_from_this();
}

}