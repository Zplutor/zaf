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
#include <zaf/window/tooltip_window.h>
#include <zaf/window/invalid_handle_state_error.h>
#include <zaf/window/message/hit_test_message.h>
#include <zaf/window/message/keyboard_message.h>
#include <zaf/window/message/message.h>
#include <zaf/window/message/mouse_message.h>
#include <zaf/window/window_class_registry.h>
#include <zaf/window/window_holder.h>

namespace zaf {
namespace {

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
        auto window = reinterpret_cast<Window*>(create_struct->lpCreateParams);
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


Window::Window(const std::wstring& window_class_name) {
    class_ = WindowClassRegistry::Instance().GetOrRegisterWindowClass(window_class_name, nullptr);
}


Window::Window(const std::shared_ptr<WindowClass>& window_class) : class_(window_class) {
    ZAF_EXPECT(class_);
}


Window::~Window() {
    // The window handle won't be destroyed in the destructor, as it is destroyed by the 
    // WindowHolder.
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

        auto holder = std::make_shared<WindowHolder>(shared_from_this());
        holder_ = holder;
        try {
            InnerCreateHandle();
            return holder;
        }
        catch (...) {

            // If the state is Destroyed, it means Destroy method has been called during the 
            // creation, so don't reset the state to NotCreated.
            if (handle_state_ != WindowHandleState::Destroyed) {
                auto auto_reset = MakeAutoReset(destroy_reason_, DestroyReason::CreationFailed);
                this->Destroy();
                handle_state_ = WindowHandleState::NotCreated;
            }

            // The holder member should be reset last, as it is used in the Destroy() method to 
            // detach the window.
            holder_.reset();
            throw;
        }
    }
    else if (handle_state_ == WindowHandleState::Created) {
        return holder_.lock();
    }
    else {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }
}


void Window::InnerCreateHandle() {

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

    handle_state_ = WindowHandleState::Creating;

    //Revise HasTitleBar property first.
    ReviseHasTitleBar();

    DWORD style = 0;
    DWORD extra_style = 0;
    GetHandleStyles(style, extra_style);

    // During the execution of CreateWindowEx, the window handle will be set to handle_ member
    // when the WM_NCCREATE message is received.
    auto handle = CreateWindowEx(
        extra_style,
        reinterpret_cast<LPCWSTR>(class_->GetAtom()),
        Title().c_str(),
        style,
        0,
        0,
        0,
        0,
        owner ? owner->Handle() : nullptr,
        nullptr,
        nullptr,
        this);

    // If CreateWindowEx returns a null handle, it means the handle has been destroyed, handle_ 
    // member should be reset to null.
    if (!handle) {
        handle_ = nullptr;
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }

    handle_state_ = WindowHandleState::Created;

    auto dpi = static_cast<float>(GetDpiForWindow(handle_));

    RECT window_rect{};
    GetWindowRect(handle_, &window_rect);
    rect_ = ToDIPs(Rect::FromRECT(window_rect), dpi);

    RECT client_rect{};
    GetClientRect(handle_, &client_rect);
    root_control_->SetRect(ToDIPs(Rect::FromRECT(client_rect), dpi));

    CreateRenderer();

    OnHandleCreated(HandleCreatedInfo{ shared_from_this() });
}


void Window::AttachHandle(HWND handle) noexcept {
    handle_ = handle;
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
        DestroyWindow(handle_);
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

    auto dpi = static_cast<float>(GetDpiForWindow(message.WindowHandle()));
    auto initial_rect = GetInitialRect(dpi);
    auto rect_in_pixels = SnapAndTransformToPixels(initial_rect, dpi);

    SetWindowPos(
        message.WindowHandle(),
        nullptr,
        static_cast<int>(rect_in_pixels.position.x),
        static_cast<int>(rect_in_pixels.position.y),
        static_cast<int>(rect_in_pixels.size.width),
        static_cast<int>(rect_in_pixels.size.height),
        SWP_NOZORDER | SWP_NOACTIVATE);

    return 0;
}


Rect Window::GetInitialRect(float dpi) const {

    auto initial_rect_style = InitialRectStyle();
    if (initial_rect_style == InitialRectStyle::Custom) {
        return rect_;
    }

    auto owner = Owner();

    if ((initial_rect_style == InitialRectStyle::CenterInOwner) && 
        (owner != nullptr)) {

        auto owner_rect = owner->Rect();
        Point position(
            owner_rect.position.x + (owner_rect.size.width - rect_.size.width) / 2,
            owner_rect.position.y + (owner_rect.size.height - rect_.size.height) / 2);

        return zaf::Rect(position, rect_.size);
    }

    //TODO: Get current monitor's size.
    float screen_width = ToDIPs(static_cast<float>(GetSystemMetrics(SM_CXSCREEN)), dpi);
    float screen_height = ToDIPs(static_cast<float>(GetSystemMetrics(SM_CYSCREEN)), dpi);
    Point position(
        (screen_width - rect_.size.width) / 2,
        (screen_height - rect_.size.height) / 2);

    return zaf::Rect(position, rect_.size);
}


void Window::CreateRenderer() {

    renderer_ = GraphicFactory::Instance().CreateWindowRenderer(handle_);
}


void Window::RecreateRenderer() {

    root_control_->ReleaseRendererResources();
    CreateRenderer();
}


void Window::GetHandleStyles(DWORD& handle_style, DWORD& handle_extra_style) const {

    handle_style |= IsPopup() ? WS_POPUP : WS_OVERLAPPED;

    if (HasBorder()) {
        
        handle_style |= WS_BORDER;

        if (HasTitleBar()) {
            handle_style |= WS_CAPTION;
        }

        if (IsSizable()) {
            handle_style |= WS_SIZEBOX;
        }
    }

    if (HasSystemMenu()) {
        handle_style |= WS_SYSMENU;
    }

    if (CanMinimize()) {
        handle_style |= WS_MINIMIZEBOX;
    }

    if (CanMaximize()) {
        handle_style |= WS_MAXIMIZEBOX;
    }

    if (IsToolWindow()) {
        handle_extra_style |= WS_EX_TOOLWINDOW;
    }

    if (IsTopmost()) {
        handle_extra_style |= WS_EX_TOPMOST;
    }

    auto activate_option = ActivateOption();
    if ((activate_option & ActivateOption::NoActivate) == ActivateOption::NoActivate) {
        handle_extra_style |= WS_EX_NOACTIVATE;
    }
}


void Window::Show() {

    auto activate_option = ActivateOption();
    bool no_activate = 
        (activate_option & ActivateOption::NoActivate) == ActivateOption::NoActivate;
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
    ShowWindow(handle_, show_command);
}


void Window::Maximize() {
    InnerShowWindow(SW_SHOWMAXIMIZED);
}


bool Window::IsWindowMaximized() const noexcept {
    if (handle_) {
        return IsMaximized(handle_);
    }
    return false;
}


bool Window::CanMaximize() const noexcept {
    if (handle_state_ == WindowHandleState::Destroyed) {
        return false;
    }
    return can_maximize_;
}


void Window::SetCanMaximize(bool has_maximize_button) {

    if (handle_state_ == WindowHandleState::Destroyed) {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }

    SetStyleProperty(can_maximize_, WS_MAXIMIZEBOX, has_maximize_button, false);
}


void Window::Minimize() {
    InnerShowWindow(SW_SHOWMINIMIZED);
}


bool Window::IsWindowMinimized() const noexcept {
    if (handle_) {
        return IsMinimized(handle_);
    }
    return false;
}


bool Window::CanMinimize() const noexcept {
    if (handle_state_ == WindowHandleState::Destroyed) {
        return false;
    }
    return can_minimize_;
}


void Window::SetCanMinimize(bool can_minimize) {
    if (handle_state_ == WindowHandleState::Destroyed) {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }
    SetStyleProperty(can_minimize_, WS_MINIMIZEBOX, can_minimize, false);
}


void Window::Restore() {
    InnerShowWindow(SW_RESTORE);
}


void Window::Hide() noexcept {

    if (handle_state_ == WindowHandleState::Created ||
        handle_state_ == WindowHandleState::Creating ||
        handle_state_ == WindowHandleState::Destroying) {
        ShowWindow(handle_, SW_HIDE);
    }
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
        auto dpi = this->GetDPI();
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
        auto activate_option = ActivateOption();
        bool no_activate = (activate_option & ActivateOption::NoActivate) == ActivateOption::NoActivate;
        bool discard_message = (activate_option & ActivateOption::DiscardMouseMessage) == ActivateOption::DiscardMouseMessage;
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

    zaf::Rect dirty_rect;

    RECT win32_rect{};
    if (GetUpdateRect(handle_, &win32_rect, TRUE)) {
        dirty_rect = ToDIPs(Rect::FromRECT(win32_rect), GetDPI());
    }
    else {
        dirty_rect = root_control_->Rect();
    }

    //The update rect must be validated before painting.
    //Because some controls may call NeedRepaint while it is painting,
    //and this may fails if there is an invalidated update rect.
    ValidateRect(handle_, nullptr);

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

    if (handle_ != nullptr) {
        RECT win32_rect = SnapAndTransformToPixels(rect, GetDPI()).ToRECT();
        InvalidateRect(handle_, &win32_rect, FALSE);
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

    zaf::Rect root_control_rect{ Point(), ToDIPs(size, GetDPI()) };
    root_control_->SetRect(root_control_rect);

    UpdateWindowRect();
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

    UpdateWindowRect();
}


void Window::UpdateWindowRect() {

    RECT rect{};
    GetWindowRect(handle_, &rect);
    rect_ = ToDIPs(Rect::FromRECT(rect), GetDPI());
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
    track_mouse_event_param.hwndTrack = handle_;

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

    HWND old_handle = handle_;
    handle_ = nullptr;
    handle_specific_state_ = {};
    renderer_ = {};
    tooltip_window_.reset();

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


std::shared_ptr<Window> Window::Owner() const noexcept {
    return owner_.lock();
}


void Window::SetOwner(const std::shared_ptr<Window>& owner) {

    if (!Handle()) {
        owner_ = owner;
    }
}


InitialRectStyle Window::InitialRectStyle() const {
    return initial_rect_style_;
}


void Window::SetInitialRectStyle(zaf::InitialRectStyle initial_rect_style) {
    initial_rect_style_ = initial_rect_style;
}


Rect Window::Rect() const {

    if (!Handle()) {
        return rect_;
    }
    else {

        RECT rect{};
        GetWindowRect(handle_, &rect);
        return ToDIPs(Rect::FromRECT(rect), GetDPI());
    }
}


void Window::SetRect(const zaf::Rect& rect) {

    if (!Handle()) {
        rect_ = rect;
    }
    else {

        auto new_rect = SnapAndTransformToPixels(rect, GetDPI());

        SetWindowPos(
            handle_,
            nullptr,
            static_cast<int>(new_rect.position.x),
            static_cast<int>(new_rect.position.y),
            static_cast<int>(new_rect.size.width),
            static_cast<int>(new_rect.size.height),
            SWP_NOZORDER | SWP_NOACTIVATE);
    }
}


zaf::Rect Window::ContentRect() const {
    return zaf::Rect{ Point{}, ContentSize() };
}


zaf::Size Window::ContentSize() const {

    if (Handle()) {

        RECT client_rect{};
        ::GetClientRect(handle_, &client_rect);
        return ToDIPs(Rect::FromRECT(client_rect).size, GetDPI());
    }
    else {

        auto adjusted_size = AdjustContentSizeToWindowSize(zaf::Size{});

        auto result = Size();
        result.width = (std::max)(result.width - adjusted_size.width, 0.f);
        result.height = (std::max)(result.height - adjusted_size.height, 0.f);
        return result;
    }
}


void Window::SetContentSize(const zaf::Size& size) {

    zaf::Rect new_rect;
    new_rect.position = Position();
    new_rect.size = AdjustContentSizeToWindowSize(size);

    SetRect(new_rect);
}


zaf::Size Window::AdjustContentSizeToWindowSize(const zaf::Size& content_size) const {

    auto dpi = GetDPI();

    zaf::Rect rect{ zaf::Point{}, content_size };
    auto rounded_rect = SnapAndTransformToPixels(rect, dpi);
    auto adjusted_rect = rounded_rect.ToRECT();

    DWORD style{};
    DWORD extra_style{};
    GetHandleStyles(style, extra_style);

    BOOL is_succeeded = AdjustWindowRectExForDpi(
        &adjusted_rect, 
        style, 
        FALSE, 
        extra_style,
        static_cast<UINT>(dpi));

    if (!is_succeeded) {
        ZAF_THROW_WIN32_ERROR(GetLastError());
    }

    auto result = zaf::Size{
        static_cast<float>(adjusted_rect.right - adjusted_rect.left),
        static_cast<float>(adjusted_rect.bottom - adjusted_rect.top)
    };

    result = ToDIPs(result, dpi);
    return result;
}


float Window::MinWidth() const {

    if (min_width_) {
        return *min_width_;
    }
    return static_cast<float>(ToDIPs(
        static_cast<float>(GetSystemMetrics(SM_CXMINTRACK)),
        GetDPI()));
}


void Window::SetMinWidth(float min_width) {

    min_width_ = min_width;

    if (MaxWidth() < min_width) {
        SetMaxWidth(min_width);
    }

    if (Width() < min_width) {
        SetWidth(min_width);
    }
}


float Window::MaxWidth() const {

    if (max_width_) {
        return *max_width_;
    }
    return static_cast<float>(ToDIPs(
        static_cast<float>(GetSystemMetrics(SM_CXMAXTRACK)),
        GetDPI()));
}


void Window::SetMaxWidth(float max_width) {

    max_width_ = max_width;

    if (MinWidth() > max_width) {
        SetMinWidth(max_width);
    }

    if (Width() > max_width) {
        SetWidth(max_width);
    }
}


float Window::MinHeight() const {

    if (min_height_) {
        return *min_height_;
    }
    return static_cast<float>(ToDIPs(
        static_cast<float>(GetSystemMetrics(SM_CYMINTRACK)),
        GetDPI()));
}


void Window::SetMinHeight(float min_height) {

    min_height_ = min_height;

    if (MaxHeight() < min_height) {
        SetMaxHeight(min_height);
    }

    if (Height() < min_height) {
        SetHeight(min_height);
    }
}


float Window::MaxHeight() const {

    if (max_height_) {
        return *max_height_;
    }
    return static_cast<float>(ToDIPs(
        static_cast<float>(GetSystemMetrics(SM_CYMAXTRACK)), 
        GetDPI()));
}


void Window::SetMaxHeight(float max_height) {

    max_height_ = max_height;

    if (MinHeight() > max_height) {
        SetMinHeight(max_height);
    }

    if (Height() > max_height) {
        SetHeight(max_height);
    }
}


ActivateOption Window::ActivateOption() const {
    return activate_option_;
}

void Window::SetActivateOption(zaf::ActivateOption option) {
    if (!Handle()) {
        activate_option_ = option;
    }
}


bool Window::IsPopup() const {
    return is_popup_;
}

void Window::SetIsPopup(bool is_popup) {

    if (!Handle()) {
        is_popup_ = is_popup;
        ReviseHasTitleBar();
    }
}


bool Window::HasBorder() const {
    return has_border_;
}

void Window::SetHasBorder(bool has_border) {

    if (!Handle()) {
        has_border_ = has_border;
        ReviseHasTitleBar();
    }
}


bool Window::HasTitleBar() const {
    return has_title_bar_;
}


void Window::SetHasTitleBar(bool has_title_bar) {

    //If the handle has been created, and it is an overlapped window and has border,
    //not allow to change this property.
    //If the handle is not created yet, allow to change this property, but it will be
    //revised when creating handle.
    if (!!Handle()) {
        if (!IsPopup() && HasBorder()) {
            return;
        }
    }

    has_title_bar_ = has_title_bar;

    if (HasBorder()) {
        SetStyleToHandle(WS_CAPTION, has_title_bar, false);
    }
}


void Window::ReviseHasTitleBar() {
    if (!IsPopup() && HasBorder()) {
        SetHasTitleBar(true);
    }
}


bool Window::IsSizable() const {
    return is_sizable_;
}

void Window::SetIsSizable(bool is_sizable) {

    is_sizable_ = is_sizable;

    if (HasBorder()) {
        SetStyleToHandle(WS_SIZEBOX, is_sizable, false);
    }
}


bool Window::HasSystemMenu() const {
    return has_system_menu_;
}

void Window::SetHasSystemMenu(bool has_system_menu) {
    SetStyleProperty(has_system_menu_, WS_SYSMENU, has_system_menu, false);
}


bool Window::IsToolWindow() const {
    return is_tool_window_;
}

void Window::SetIsToolWindow(bool is_tool_window) {
    SetStyleProperty(is_tool_window_, WS_EX_TOOLWINDOW, is_tool_window, true);
}


bool Window::IsTopmost() const {
    return is_topmost_;
}


void Window::SetIsTopmost(bool is_topmost) {
    SetStyleProperty(is_topmost_, WS_EX_TOPMOST, is_topmost, true);
}


void Window::SetStyleProperty(
    bool& property_value,
    DWORD style_value,
    bool is_set,
    bool is_extra_style) {

    property_value = is_set;

    if (Handle()) {
        SetStyleToHandle(style_value, is_set, is_extra_style);
    }
}


void Window::SetStyleToHandle(DWORD style_value, bool is_set, bool is_extra_style) {

    DWORD category = is_extra_style ? GWL_EXSTYLE : GWL_STYLE;

    DWORD style = GetWindowLong(Handle(), category);
    if (is_set) {
        style |= style_value;
    }
    else {
        style &= ~style_value;
    }
    SetWindowLong(Handle(), category, style);
}


std::wstring Window::Title() const {

    if (!Handle()) {
        return title_;
    }
    else {

        int title_length = GetWindowTextLength(handle_);
        std::vector<wchar_t> buffer(title_length + 1);
        GetWindowText(handle_, buffer.data(), static_cast<int>(buffer.size()));
        return buffer.data();
    }
}


void Window::SetTitle(const std::wstring& title) {

    title_ = title;

    if (Handle()) {
        SetWindowText(handle_, title_.c_str());
    }
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

    if (!!Handle()) {
        RECT client_rect{};
        ::GetClientRect(handle_, &client_rect);
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
    ScreenToClient(handle_, &cursor_point);

    Point point_in_pixels{ 
        static_cast<float>(cursor_point.x), 
        static_cast<float>(cursor_point.y) 
    };

    return ToDIPs(point_in_pixels, GetDPI());
}


Point Window::TranslateToScreen(const Point& position) const {

    auto handle = Handle();
    ZAF_EXPECT(handle);

    auto dpi = GetDPI();
    auto position_in_pixel = FromDIPs(position, dpi);

    auto point = position_in_pixel.ToPOINT();
    ClientToScreen(handle, &point);

    auto result = Point::FromPOINT(point);
    return ToDIPs(result, dpi);
}


Point Window::TranslateFromScreen(const Point& position_in_screen) const {

    auto handle = Handle();
    ZAF_EXPECT(handle);

    auto dpi = GetDPI();
    auto position_in_pixel = FromDIPs(position_in_screen, dpi);

    auto point = position_in_pixel.ToPOINT();
    ScreenToClient(handle, &point);

    auto result = Point::FromPOINT(point);
    return ToDIPs(result, dpi);
}


float Window::GetDPI() const {

    if (Handle()) {
        return static_cast<float>(GetDpiForWindow(Handle()));
    }
    else {
        return Application::Instance().GetSystemDPI();
    }
}


bool Window::Activate() {
    ZAF_EXPECT(Handle());
    return !!SetForegroundWindow(Handle());
}


bool Window::IsVisible() const noexcept {
    return !!IsWindowVisible(handle_);
}


bool Window::IsFocused() const {

    if (!handle_) {
        return false;
    }

    return GetFocus() == handle_;
}


void Window::Close() {

    //Do nothing if the window is being destroyed.
    if (handle_state_ == WindowHandleState::Destroying) {
        return;
    }

    SendMessage(Handle(), WM_CLOSE, 0, 0);
}


WindowMessager Window::Messager() {
    ZAF_EXPECT(handle_);
    return WindowMessager{ handle_ };
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