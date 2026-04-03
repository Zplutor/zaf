#include <zaf/window/window.h>
#include <Windowsx.h>
#include <zaf/application.h>
#include <zaf/base/auto_reset.h>
#include <zaf/base/error/win32_error.h>
#include <zaf/creation.h>
#include <zaf/graphic/pixel_snapping.h>
#include <zaf/graphic/dpi.h>
#include <zaf/internal/tab_stop_utility.h>
#include <zaf/window/internal/window_facets/window_control_facet.h>
#include <zaf/window/internal/window_facets/window_focus_facet.h>
#include <zaf/window/internal/window_facets/window_geometry_facet.h>
#include <zaf/window/internal/window_facets/window_inspect_facet.h>
#include <zaf/window/internal/window_facets/window_keyboard_facet.h>
#include <zaf/window/internal/window_facets/window_lifecycle_facet.h>
#include <zaf/window/internal/window_facets/window_mouse_facet.h>
#include <zaf/window/internal/window_facets/window_render_facet.h>
#include <zaf/window/internal/window_facets/window_style_facet.h>
#include <zaf/window/internal/window_facets/window_visibility_facet.h>
#include <zaf/window/internal/window_style_shim.h>
#include <zaf/window/invalid_handle_state_error.h>
#include <zaf/window/message/hit_test_message.h>
#include <zaf/window/message/keyboard_message.h>
#include <zaf/window/message/message.h>
#include <zaf/window/message/mouse_message.h>
#include <zaf/window/screen_manager.h>
#include <zaf/window/window_class_registry.h>
#include <zaf/window/window_holder.h>

namespace zaf {

ZAF_OBJECT_IMPL(Window);

Window::Window() : Window(WindowClassRegistry::Instance().GetDefaultWindowClass()) {

}


Window::Window(std::wstring_view window_class_name) : 
    Window(WindowClassRegistry::Instance().GetOrRegisterWindowClass(window_class_name, nullptr)) {

}


Window::Window(std::shared_ptr<WindowClass> window_class) :
    style_facet_(std::make_unique<internal::WindowStyleFacet>(*this, std::move(window_class))),
    geometry_facet_(std::make_unique<internal::WindowGeometryFacet>(*this)),
    lifecycle_facet_(std::make_unique<internal::WindowLifecycleFacet>(*this)),
    visibility_facet_(std::make_unique<internal::WindowVisibilityFacet>(*this)),
    control_facet_(std::make_unique<internal::WindowControlFacet>(*this)),
    render_facet_(std::make_unique<internal::WindowRenderFacet>(*this)),
    focus_facet_(std::make_unique<internal::WindowFocusFacet>(*this)),
    mouse_facet_(std::make_unique<internal::WindowMouseFacet>(*this)),
    keyboard_facet_(std::make_unique<internal::WindowKeyboardFacet>(*this)),
    inspect_facet_(std::make_unique<internal::WindowInspectFacet>(*this)) {

}


Window::~Window() {
    // The window handle won't be destroyed in the destructor, as it is destroyed by the 
    // WindowHolder.
}


void Window::Initialize() {

    __super::Initialize();

    control_facet_->OnWindowInitialize();
}


std::shared_ptr<Window> Window::Owner() const noexcept {
    return owner_.lock();
}


void Window::SetOwner(std::shared_ptr<Window> owner) {

    if (HandleState() != WindowHandleState::NotCreated) {
        throw InvalidHandleStateError(ZAF_SOURCE_LOCATION());
    }

    if (this == owner.get()) {
        throw InvalidOperationError(ZAF_SOURCE_LOCATION());
    }

    owner_ = std::move(owner);
}


void Window::ShowInspector() const {
    inspect_facet_->ShowInspector();
}


#pragma region Style Management

const std::shared_ptr<WindowClass>& Window::Class() const noexcept {
    return style_facet_->Class();
}


std::wstring Window::Title() const {
    return style_facet_->Title();
}


void Window::SetTitle(const std::wstring& title) {
    style_facet_->SetTitle(title);
}


bool Window::IsPopup() const noexcept {
    return style_facet_->IsPopup();
}


void Window::SetIsPopup(bool is_popup) {
    style_facet_->SetIsPopup(is_popup);
}


bool Window::HasBorder() const noexcept {
    return style_facet_->HasBorder();
}


void Window::SetHasBorder(bool has_border) {
    style_facet_->SetHasBorder(has_border);
}


bool Window::HasTitleBar() const noexcept {
    return style_facet_->HasTitleBar();
}


void Window::SetHasTitleBar(bool has_title_bar) {
    style_facet_->SetHasTitleBar(has_title_bar);
}


bool Window::HasSystemMenu() const noexcept {
    return style_facet_->HasSystemMenu();
}


void Window::SetHasSystemMenu(bool has_system_menu) {
    style_facet_->SetHasSystemMenu(has_system_menu);
}


bool Window::IsSizable() const noexcept {
    return style_facet_->IsSizable();
}


void Window::SetIsSizable(bool is_sizable) {
    style_facet_->SetIsSizable(is_sizable);
}


bool Window::CanMaximize() const noexcept {
    return style_facet_->CanMaximize();
}


void Window::SetCanMaximize(bool has_maximize_button) {
    style_facet_->SetCanMaximize(has_maximize_button);
}


bool Window::CanMinimize() const noexcept {
    return style_facet_->CanMinimize();
}


void Window::SetCanMinimize(bool can_minimize) {
    style_facet_->SetCanMinimize(can_minimize);
}


bool Window::IsToolWindow() const noexcept {
    return style_facet_->IsToolWindow();
}


void Window::SetIsToolWindow(bool is_tool_window) {
    style_facet_->SetIsToolWindow(is_tool_window);
}


bool Window::IsTopmost() const noexcept {
    return style_facet_->IsTopmost();
}


void Window::SetIsTopmost(bool is_topmost) {
    style_facet_->SetIsTopmost(is_topmost);
}


ActivateOptions Window::ActivateOptions() const noexcept {
    return style_facet_->ActivateOptions();
}


void Window::SetActivateOptions(zaf::ActivateOptions options) {
    style_facet_->SetActivateOptions(options);
}


bool Window::UseCustomFrame() const noexcept {
    return style_facet_->UseCustomFrame();
}


void Window::SetUseCustomFrame(bool use_custom_frame) noexcept {
    style_facet_->SetUseCustomFrame(use_custom_frame);
}

#pragma endregion


#pragma region Geometry Management

std::shared_ptr<zaf::Screen> Window::Screen() const noexcept {
    return geometry_facet_->Screen();
}


void Window::SetScreen(std::shared_ptr<zaf::Screen> screen) {
    geometry_facet_->SetScreen(std::move(screen));
}


float Window::DPI() const noexcept {
    return geometry_facet_->DPI();
}


zaf::Rect Window::Rect() const noexcept {
    return geometry_facet_->Rect();
}


void Window::SetRect(const zaf::Rect& rect) {
    geometry_facet_->SetRect(rect);
}


Point Window::Position() const noexcept {
    return geometry_facet_->Position();
}


void Window::SetPosition(const Point& position) {
    geometry_facet_->SetPosition(position);
}


zaf::Size Window::Size() const noexcept {
    return geometry_facet_->Size();
}

void Window::SetSize(const zaf::Size& size) {
    geometry_facet_->SetSize(size);
}


float Window::Width() const noexcept {
    return geometry_facet_->Width();
}


void Window::SetWidth(float width) {
    geometry_facet_->SetWidth(width);
}


float Window::Height() const noexcept {
    return geometry_facet_->Height();
}


void Window::SetHeight(float height) {
    geometry_facet_->SetHeight(height);
}


float Window::MinWidth() const noexcept {
    return geometry_facet_->MinWidth();
}


void Window::SetMinWidth(float min_width) {
    geometry_facet_->SetMinWidth(min_width);
}


float Window::MaxWidth() const noexcept {
    return geometry_facet_->MaxWidth();
}


void Window::SetMaxWidth(float max_width) {
    geometry_facet_->SetMaxWidth(max_width);
}


float Window::MinHeight() const noexcept {
    return geometry_facet_->MinHeight();
}


void Window::SetMinHeight(float min_height) {
    geometry_facet_->SetMinHeight(min_height);
}


float Window::MaxHeight() const noexcept {
    return geometry_facet_->MaxHeight();
}


void Window::SetMaxHeight(float max_height) {
    geometry_facet_->SetMaxHeight(max_height);
}


zaf::Size Window::MinSize() const noexcept {
    return geometry_facet_->MinSize();
}


void Window::SetMinSize(const zaf::Size& size) {
    geometry_facet_->SetMinSize(size);
}


zaf::Size Window::MaxSize() const noexcept {
    return geometry_facet_->MaxSize();
}


void Window::SetMaxSize(const zaf::Size& size) {
    geometry_facet_->SetMaxSize(size);
}


zaf::Rect Window::ContentRect() const noexcept {
    return geometry_facet_->ContentRect();
}


zaf::Size Window::ContentSize() const noexcept {
    return geometry_facet_->ContentSize();
}


void Window::SetContentSize(const zaf::Size& size) {
    geometry_facet_->SetContentSize(size);
}


float Window::ContentWidth() const noexcept {
    return geometry_facet_->ContentWidth();
}


void Window::SetContentWidth(float width) {
    geometry_facet_->SetContentWidth(width);
}


float Window::ContentHeight() const noexcept {
    return geometry_facet_->ContentHeight();
}


void Window::SetContentHeight(float height) {
    geometry_facet_->SetContentHeight(height);
}


void Window::OnSizeChanged(const WindowSizeChangedInfo& event_info) {
    size_changed_event_.Raise(event_info);
}


rx::Observable<WindowSizeChangedInfo> Window::SizeChangedEvent() const {
    return size_changed_event_.GetObservable();
}


void Window::OnDPIChanged(const DPIChangedInfo& event_info) {
    dpi_changed_event_.Raise(event_info);
}


rx::Observable<DPIChangedInfo> Window::DPIChangedEvent() const {
    return dpi_changed_event_.GetObservable();
}


bool Window::IsSizingOrMoving() const noexcept {
    return geometry_facet_->IsSizingOrMoving();
}


rx::Single<None> Window::WhenNotSizingOrMoving() const {
    return geometry_facet_->WhenNotSizingOrMoving();
}


Point Window::TransformToScreen(const Point& position) const noexcept {
    return geometry_facet_->TransformToScreen(position);
}


Point Window::TransformFromScreen(const Point& position_in_screen) const noexcept {
    return geometry_facet_->TransformFromScreen(position_in_screen);
}


zaf::Rect Window::TransformToScreen(const zaf::Rect& rect_in_window) const noexcept {
    return geometry_facet_->TransformToScreen(rect_in_window);
}


zaf::Rect Window::TransformFromScreen(const zaf::Rect& rect_in_screen) const noexcept {
    return geometry_facet_->TransformFromScreen(rect_in_screen);
}

#pragma endregion


#pragma region Lifecycle Management

WindowHandleState Window::HandleState() const noexcept {
    return lifecycle_facet_->HandleState();
}


HWND Window::Handle() const noexcept {
    return lifecycle_facet_->Handle();
}


std::shared_ptr<WindowHolder> Window::CreateHandle() {
    return lifecycle_facet_->CreateHandle();
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
    lifecycle_facet_->Destroy();
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


void Window::Close() noexcept {
    lifecycle_facet_->Close();
}


void Window::OnClosing(const ClosingInfo& event_info) {
    closing_event_.Raise(event_info);
}


rx::Observable<ClosingInfo> Window::ClosingEvent() const {
    return closing_event_.GetObservable();
}

#pragma endregion


#pragma region Visibility Management

void Window::Show(ShowOptions options) {
    visibility_facet_->Show(options);
}


void Window::Maximize() {
    visibility_facet_->Maximize();
}


bool Window::IsWindowMaximized() const noexcept {
    return visibility_facet_->IsWindowMaximized();
}


void Window::Minimize() {
    visibility_facet_->Minimize();
}


bool Window::IsWindowMinimized() const noexcept {
    return visibility_facet_->IsWindowMinimized();
}


void Window::Restore() {
    visibility_facet_->Restore();
}


void Window::Hide() noexcept {
    visibility_facet_->Hide();
}


bool Window::IsVisible() const noexcept {
    return visibility_facet_->IsVisible();
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

#pragma endregion


#pragma region Control Management

const std::shared_ptr<Control>& Window::RootControl() const noexcept {
    return control_facet_->RootControl();
}


void Window::SetRootControl(const std::shared_ptr<Control>& control) {
    control_facet_->SetRootControl(control);
}


void Window::OnRootControlChanged(const RootControlChangedInfo& event_info) {
    root_control_changed_event_.Raise(event_info);
}


rx::Observable<RootControlChangedInfo> Window::RootControlChangedEvent() const {
    return root_control_changed_event_.GetObservable();
}

#pragma endregion


#pragma region Focus Management

bool Window::BringToForeground() noexcept {
    return focus_facet_->BringToForeground();
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


bool Window::IsFocused() const noexcept {
    return focus_facet_->IsFocused();
}


void Window::OnFocusGained(const WindowFocusGainedInfo& event_info) {
    focus_gained_event_.Raise(event_info);
}


rx::Observable<WindowFocusGainedInfo> Window::FocusGainedEvent() const {
    return focus_gained_event_.GetObservable();
}


void Window::OnFocusLost(const WindowFocusLostInfo& event_info) {
    focus_lost_event_.Raise(event_info);
}


rx::Observable<WindowFocusLostInfo> Window::FocusLostEvent() const {
    return focus_lost_event_.GetObservable();
}


std::shared_ptr<Control> Window::FocusedControl() const noexcept {
    return focus_facet_->FocusedControl();
}


void Window::OnFocusedControlChanged(const FocusedControlChangedInfo& event_info) {
    focused_control_changed_event_.Raise(event_info);
}


rx::Observable<FocusedControlChangedInfo> Window::FocusedControlChangedEvent() const {
    return focused_control_changed_event_.GetObservable();
}

#pragma endregion


#pragma region Rendering Management

void Window::RequestRepaint() noexcept {
    render_facet_->RequestRepaint();
}


void Window::RequestRepaint(const zaf::Rect& repaint_rect) noexcept {
    render_facet_->RequestRepaint(repaint_rect);
}


void Window::RepaintIfNeeded() noexcept {
    render_facet_->RepaintIfNeeded();
}

#pragma endregion


#pragma region Mouse Input Handling

std::shared_ptr<Control> Window::MouseOverControl() const noexcept {
    return mouse_facet_->MouseOverControl();
}


std::shared_ptr<Control> Window::MouseCaptureControl() const noexcept {
    return mouse_facet_->MouseCaptureControl();
}


void Window::OnMouseCaptureControlChanged(const MouseCaptureControlChangedInfo& event_info) {
    mouse_capture_control_changed_event_.Raise(event_info);
}


rx::Observable<MouseCaptureControlChangedInfo> Window::MouseCaptureControlChangedEvent() const {
    return mouse_capture_control_changed_event_.GetObservable();
}


std::optional<HitTestResult> Window::HitTest(const HitTestMessage& message) {
    return mouse_facet_->HitTest(message);
}


Point Window::MousePosition() const noexcept {
    return mouse_facet_->MousePosition();
}

#pragma endregion


#pragma region Keyboard Input Handling

bool Window::PreprocessMessage(const KeyMessage& message) {
    return keyboard_facet_->PreprocessMessage(message);
}

#pragma endregion


#pragma region Message Handling

LRESULT CALLBACK Window::WindowProcedure(
    HWND hwnd,
    UINT message_id,
    WPARAM wparam,
    LPARAM lparam) {

    if (message_id == WM_NCCREATE) {
        internal::WindowLifecycleFacet::HandleWMNCCREATE(hwnd, lparam);
    }

    auto window = GetWindowFromHandle(hwnd);
    if (window) {
        return window->RouteWindowMessage(hwnd, message_id, wparam, lparam);
    }

    return CallWindowProc(DefWindowProc, hwnd, message_id, wparam, lparam);
}


LRESULT Window::RouteWindowMessage(HWND hwnd, UINT id, WPARAM wparam, LPARAM lparam) {

    Message message{ hwnd, id, wparam, lparam };
    auto shared_this = shared_from_this();

    // Raise the message handling event first.
    bool can_be_interrupted = CanMessageBeInterupted(id);
    MessageHandlingInfo message_handling_info{ shared_this, message, can_be_interrupted };
    OnMessageHandling(message_handling_info);

    // Check if the message has been handled, if not, handle it.
    auto handle_result = message_handling_info.HandleResult();
    if (!handle_result) {

        // The window may be destroyed in the message handling event, in such case it cannot handle
        // the message, otherwise it will cause undefined behavior. WM_NCDESTROY is the only 
        // exception.
        bool can_handle_message = 
            (HandleState() != WindowHandleState::Destroyed) || 
            (id == WM_NCDESTROY);

        if (can_handle_message) {
            handle_result = HandleMessage(message);
        }
        
        // If still not handled, call the default window procedure.
        if (!handle_result) {
            handle_result = CallWindowProc(DefWindowProc, hwnd, id, wparam, lparam);
        }
    }

    // Raise the message handled event then.
    MessageHandledInfo message_handled_info{ shared_this, message, *handle_result };
    OnMessageHandled(message_handled_info);

    return *handle_result;
}


bool Window::CanMessageBeInterupted(const UINT message_id) noexcept {
    switch (message_id) {
    case WM_NCCREATE:
    case WM_CREATE:
    case WM_DESTROY:
    case WM_NCDESTROY:
        return false;
    default:
        return true;
    }
}


std::optional<LRESULT> Window::HandleMessage(const Message& message) {

    switch (message.ID()) {

    case WM_NCCALCSIZE:
        return style_facet_->HandleWMNCCALCSIZE(message);

    case WM_SHOWWINDOW:
        HandleWMSHOWWINDOW(ShowWindowMessage{ message });
        return 0;

#pragma region Lifecycle Messages
    case WM_CREATE:
        lifecycle_facet_->HandleWMCREATE();
        return 0;

    case WM_CLOSE:
        lifecycle_facet_->HandleWMCLOSE();
        return 0;

    case WM_DESTROY:
        lifecycle_facet_->HandleWMDESTROY();
        return 0;

    case WM_NCDESTROY:
        lifecycle_facet_->HandleWMNCDESTROY();
        return 0;
#pragma endregion

#pragma region Geometry Messages
    case WM_GETMINMAXINFO: 
        geometry_facet_->HandleWMGETMINMAXINFO(message);
        return 0;

    case WM_ENTERSIZEMOVE:
        geometry_facet_->HandleWMENTERSIZEMOVE();
        return 0;

    case WM_EXITSIZEMOVE:
        geometry_facet_->HandleWMEXITSIZEMOVE();
        return 0;

    case WM_SIZE:
        geometry_facet_->HandleWMSIZE(message);
        return 0;

    case WM_DPICHANGED:
        geometry_facet_->HandleWMDPICHANGED(message);
        return 0;
#pragma endregion

#pragma region Rendering Messages
    case WM_ERASEBKGND:
        //Don't erase background to avoid blinking.
        return TRUE;

    case WM_PAINT:
        render_facet_->HandleWMPAINT();
        return 0;
#pragma endregion

#pragma region Focus Messages
    case WM_ACTIVATE:
        focus_facet_->HandleWMACTIVATE(ActivateMessage{ message });
        return 0;

    case WM_SETFOCUS:
        focus_facet_->HandleWMSETFOCUS(message);
        return 0;

    case WM_KILLFOCUS:
        focus_facet_->HandleWMKILLFOCUS(message);
        return 0;

    case WM_MOUSEACTIVATE:
        return focus_facet_->HandleWMMOUSEACTIVATE();
#pragma endregion

#pragma region Mouse Messages
    case WM_NCHITTEST: {
        auto hit_test_result = HitTest(HitTestMessage{ message });
        if (hit_test_result) {
            return static_cast<LRESULT>(*hit_test_result);
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
        if (mouse_facet_->HandleGeneralMouseMessage(MouseMessage{ message })) {
            return 0;
        }
        return std::nullopt;
    }

    case WM_MOUSEHOVER:
    case WM_NCMOUSEHOVER:
        mouse_facet_->HandleMouseHoverMessage(message);
        return 0;

    case WM_MOUSELEAVE:
    case WM_NCMOUSELEAVE:
        mouse_facet_->HandleMouseLeaveMessage(message);
        return 0;

    case WM_MOUSEWHEEL:
    case WM_MOUSEHWHEEL: {
        if (mouse_facet_->HandleMouseWheelMessage(message)) {
            return 0;
        }
        return std::nullopt;
    }

    case WM_CAPTURECHANGED:
        mouse_facet_->CancelMouseCapture();
        return 0;

    case WM_SETCURSOR: {
        if (mouse_facet_->HandleWMSETCURSOR(message)) {
            return TRUE;
        }
        return std::nullopt;
    }
#pragma endregion

#pragma region Keyboard Messages
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_CHAR:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_SYSCHAR:
        if (keyboard_facet_->HandleKeyboardMessage(message)) {
            return 0;
        }
        return std::nullopt;

    case WM_IME_STARTCOMPOSITION:
    case WM_IME_COMPOSITION:
    case WM_IME_ENDCOMPOSITION:
    case WM_IME_REQUEST:
        return keyboard_facet_->HandleIMEMessage(message);
#pragma endregion

    default:
        return std::nullopt;
    }
}


void Window::OnMessageHandling(const MessageHandlingInfo& event_info) {
    message_handling_event_.Raise(event_info);
}


rx::Observable<MessageHandlingInfo> Window::MessageHandlingEvent() const {
    return message_handling_event_.GetObservable();
}


void Window::OnMessageHandled(const MessageHandledInfo& event_info) {
    message_handled_event_.Raise(event_info);
}


rx::Observable<MessageHandledInfo> Window::MessageHandledEvent() const {
    return message_handled_event_.GetObservable();
}


WindowMessager Window::Messager() noexcept {
    return WindowMessager{ Handle() };
}

#pragma endregion


void Window::HandleWMSHOWWINDOW(const ShowWindowMessage& message) {

    internal::ShowWindowEventInfo event_info{ shared_from_this(), message };

    if (message.IsHidden()) {
        OnHide(event_info);
    }
    else {
        OnShow(event_info);
    }
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