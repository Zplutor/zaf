#include <zaf/window/window.h>
#include <Windowsx.h>
#include <zaf/application.h>
#include <zaf/base/error/system_error.h>
#include <zaf/base/event_utility.h>
#include <zaf/creation.h>
#include <zaf/graphic/alignment.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/resource_factory.h>
#include <zaf/internal/tab_stop_utility.h>
#include <zaf/internal/theme.h>
#include <zaf/parsing/parsers/window_parser.h>
#include <zaf/object/type_definition.h>
#include <zaf/serialization/properties.h>
#include <zaf/window/caret.h>
#include <zaf/window/inspector/inspector_window.h>
#include <zaf/window/message/creation.h>
#include <zaf/window/message/hit_test_message.h>
#include <zaf/window/message/keyboard_message.h>
#include <zaf/window/message/message.h>
#include <zaf/window/message/mouse_message.h>

namespace zaf {
namespace {

const wchar_t* const kDefaultWindowClassName = L"ZafDefaultWindowClass";

constexpr wchar_t* const kActivateOptionPropertyName = L"ActivateOption";
constexpr wchar_t* const kCanMaximizePropertyName = L"CanMaximize";
constexpr wchar_t* const kCanMinimizePropertyName = L"CanMinimize";
constexpr wchar_t* const kCloseHandlerPropertyName = L"CloseHandler";
constexpr wchar_t* const kDestroyEventPropertyName = L"DestroyEvent";
constexpr wchar_t* const kHasBorderPropertyName = L"HasBorder";
constexpr wchar_t* const kHasSystemMenuPropertyName = L"HasSystemMenu";
constexpr wchar_t* const kHasTitleBarPropertyName = L"HasTitleBar";
constexpr wchar_t* const kInitialRectStylePropertyName = L"InitialRectStyle";
constexpr wchar_t* const kIsPopupPropertyName = L"IsPopup";
constexpr wchar_t* const kIsSizablePropertyName = L"IsSizable";
constexpr wchar_t* const kIsToolWindowPropertyName = L"IsToolWindow";
constexpr wchar_t* const kOwnerPropertyName = L"Owner";
constexpr wchar_t* const kTitlePropertyName = L"Title";

}

ZAF_DEFINE_TYPE(Window)
    ZAF_DEFINE_TYPE_PARSER(WindowParser)
ZAF_DEFINE_TYPE_END


void Window::RegisterDefaultClass(HICON icon, HICON small_icon) {

    WNDCLASSEX default_class = { 0 };
    default_class.cbSize = sizeof(default_class);
    default_class.style = CS_HREDRAW | CS_VREDRAW;
    default_class.lpfnWndProc = WindowProcedure;
    default_class.cbClsExtra = 0;
    default_class.cbWndExtra = sizeof(LONG_PTR);
    default_class.hInstance = NULL;
    default_class.hIcon = icon;
    default_class.hCursor = LoadCursor(NULL, IDI_APPLICATION);
    default_class.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    default_class.lpszMenuName = nullptr;
    default_class.lpszClassName = kDefaultWindowClassName;
    default_class.hIconSm = small_icon;

    ATOM atom = RegisterClassEx(&default_class);
    if (atom == 0) {
        ZAF_THROW_IF_SYSTEM_ERROR(GetLastError());
    }
}


LRESULT CALLBACK Window::WindowProcedure(HWND hwnd, UINT message_id, WPARAM wparam, LPARAM lparam) {

    if (message_id == WM_NCCREATE) {
        auto create_struct = reinterpret_cast<const CREATESTRUCTA*>(lparam);
        SetWindowLongPtr(
            hwnd, 
            GWLP_USERDATA, 
            reinterpret_cast<ULONG_PTR>(create_struct->lpCreateParams));
    }

    auto window = GetWindowFromHandle(hwnd);
    if (window) {

        auto message = CreateMessage(hwnd, message_id, wparam, lparam);

        LRESULT result = 0;
        if (window->ReceiveMessage(*message, result)) {
            return result;    
        }
    }

    return CallWindowProc(DefWindowProc, hwnd, message_id, wparam, lparam);
}


Window::Window() :
    handle_(nullptr),
    rect_(0, 0, 640, 480) {

}


Window::~Window() {

    if (handle_ != nullptr) {
        DestroyWindow(handle_);
    }
}


void Window::Initialize() {

    __super::Initialize();

    InitializeRootControl(Create<Control>());
}


void Window::CreateWindowHandle() {

    auto owner = GetOwner();
    auto initial_rect = GetInitialRect();

    DWORD style = 0;
    DWORD extra_style = 0;
    GetHandleStyles(style, extra_style);

    handle_ = CreateWindowEx(
        extra_style,
        kDefaultWindowClassName,
        GetTitle().c_str(),
        style,
        static_cast<int>(initial_rect.position.x),
        static_cast<int>(initial_rect.position.y),
        static_cast<int>(initial_rect.size.width),
        static_cast<int>(initial_rect.size.height),
        owner == nullptr ? nullptr : owner->GetHandle(),
        nullptr,
        nullptr,
        this);

    rect_ = initial_rect;

    RECT client_rect = { 0 };
    ::GetClientRect(handle_, &client_rect);
    root_control_->SetRect(Rect::FromRECT(client_rect));

    CreateRenderer();
    Application::Instance().RegisterWindow(shared_from_this());

    OnWindowCreated();
}


const Rect Window::GetInitialRect() const {

    auto initial_rect_style = GetInitialRectStyle();

    if (initial_rect_style == InitialRectStyle::Custom) {
        return rect_;
    }

    auto owner = GetOwner();

    if ((initial_rect_style == InitialRectStyle::CenterInOwner) && 
        (owner != nullptr)) {

        auto owner_rect = owner->GetRect();
        Point position(
            owner_rect.position.x + (owner_rect.size.width - rect_.size.width) / 2,
            owner_rect.position.y + (owner_rect.size.height - rect_.size.height) / 2);

        return Rect(position, rect_.size);
    }

    int screen_width = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);
    Point position(
        (screen_width - rect_.size.width) / 2,
        (screen_height - rect_.size.height) / 2);

    return Rect(position, rect_.size);
}


void Window::CreateRenderer() {

    renderer_ = GetGraphicFactory().CreateWindowRenderer(handle_);
}


void Window::RecreateRenderer() {

    root_control_->ReleaseRendererResources();
    CreateRenderer();
}


void Window::CheckCreateWindowHandle() {

    if (IsClosed()) {
        CreateWindowHandle();
    }
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

    auto activate_option = GetActivateOption();
    if ((activate_option & ActivateOption::NoActivate) == ActivateOption::NoActivate) {
        handle_extra_style |= WS_EX_NOACTIVATE;
    }
}


bool Window::PreprocessMessage(const KeyMessage& message) {

    if (TryToPreprocessTabKeyMessage(message)) {
        return true;
    }

    if (TryToPreprocessInspectorShortcutMessage(message)) {
        return true;
    }

    return false;
}


bool Window::TryToPreprocessTabKeyMessage(const KeyMessage& message) {

    if (message.id != WM_KEYDOWN || message.GetVirtualKey() != VK_TAB) {
        return false;
    }

    if ((focused_control_) && focused_control_->AcceptKeyMessage(message)) {
        return false;
    }

    bool backward = (GetKeyState(VK_SHIFT) >> 15) != 0;
    SwitchFocusedControlByTabKey(backward);
    return true;
}


void Window::SwitchFocusedControlByTabKey(bool backward) {

    auto current_focused_control = focused_control_;
    if (current_focused_control == nullptr) {
        current_focused_control = root_control_;
    }

    auto next_focused_control = internal::FindNextTabStopControl(current_focused_control, backward);
    if (next_focused_control != nullptr) {
        SetFocusedControl(next_focused_control);
    }
}


bool Window::TryToPreprocessInspectorShortcutMessage(const KeyMessage& message) {
#ifndef NDEBUG
    if (message.id != WM_KEYDOWN) {
        return false;
    }

    if (message.GetVirtualKey() != VK_F12) {
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


bool Window::ReceiveMessage(const Message& message, LRESULT& result) {

    auto is_customized_style = [this]() {
        return !IsPopup() && !HasBorder();
    };

    switch (message.id) {
    case WM_NCCALCSIZE:
        if (is_customized_style()) {
            result = TRUE;
            return true;
        }
        return false;
        
    case WM_NCACTIVATE:
        //Work around to hide the odd border when the window is activated or inactivated.
        if (is_customized_style()) {
            result = TRUE;
            return true;
        }
        return false;
        
    case WM_ERASEBKGND:
        result = TRUE;
        return true;

    case WM_PAINT:
        Repaint();
        return true;

    case WM_GETMINMAXINFO: {
        auto min_max_info = reinterpret_cast<MINMAXINFO*>(message.lparam);
        min_max_info->ptMinTrackSize.x = static_cast<LONG>(GetMinWidth());
        min_max_info->ptMinTrackSize.y = static_cast<LONG>(GetMinHeight());
        min_max_info->ptMaxTrackSize.x = static_cast<LONG>(GetMaxWidth());
        min_max_info->ptMaxTrackSize.y = static_cast<LONG>(GetMaxHeight());
        result = 0;
        return true;
    }

    case WM_SIZE:
        Resize(LOWORD(message.lparam), HIWORD(message.lparam));
        return true;

    case WM_MOUSEACTIVATE: {
        auto activate_option = GetActivateOption();
        bool no_activate = (activate_option & ActivateOption::NoActivate) == ActivateOption::NoActivate;
        bool discard_message = (activate_option & ActivateOption::DiscardMouseMessage) == ActivateOption::DiscardMouseMessage;
        if (no_activate) {
            result = discard_message ? MA_NOACTIVATEANDEAT : MA_NOACTIVATE;
        }
        else {
            result = discard_message ? MA_ACTIVATEANDEAT : MA_ACTIVATE;
        }
        return true;
    }

    case WM_CAPTURECHANGED: {
        if (capturing_mouse_control_ != nullptr) {

            auto previous_control = capturing_mouse_control_;

            capturing_mouse_control_->IsCapturingMouseChanged(false);
            capturing_mouse_control_ = nullptr;

            OnCapturingMouseControlChanged(previous_control);
        }
        result = 0;
        return true;
    }

    case WM_NCHITTEST: {
        auto hit_test_result = HitTest(dynamic_cast<const HitTestMessage&>(message));
        if (hit_test_result) {
            result = static_cast<LRESULT>(*hit_test_result);
            return true;
        }
        else {
            return false;
        }
    }

    case WM_SETCURSOR: {
        bool is_changed = ChangeMouseCursor(message);
        if (is_changed) {
            result = TRUE;
            return true;
        }
        else {
            return false;
        }
    }

    case WM_MOUSEWHEEL:
    case WM_MOUSEHWHEEL:
        //Mouse wheel messages are not sent to an unfocused window even if it captures
        //the mouse, because these messages are only sent to focused window.
        //But we wish these messages have the same behaviour as other mouse input messages,
        //so the messages are redircted to the window which is capturing the mouse.
        if (RedirectMouseWheelMessage(message)) {
            return true;
        }
        //Fall through

    case WM_MOUSEMOVE:
    case WM_NCMOUSEMOVE:
    case WM_MOUSELEAVE:
    case WM_NCMOUSELEAVE:
    case WM_LBUTTONDOWN:
    case WM_NCLBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_NCLBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
        return ReceiveMouseMessage(dynamic_cast<const MouseMessage&>(message));

    case WM_KEYDOWN: 
        if (focused_control_ != nullptr) {
            return focused_control_->OnKeyDown(dynamic_cast<const KeyMessage&>(message));
        }
        return false;

    case WM_KEYUP:
        if (focused_control_ != nullptr) {
            return focused_control_->OnKeyUp(dynamic_cast<const KeyMessage&>(message));
        }
        return false;

    case WM_CHAR:
        if (focused_control_ != nullptr) {
            return focused_control_->OnCharInput(dynamic_cast<const CharMessage&>(message));
        }
        return false;

    case WM_CLOSE:
        return ! ReceiveCloseMessage();

    case WM_DESTROY:
        ReceiveDestroyMessage();
        return true;

    case WM_NCDESTROY:
        Application::Instance().UnregisterWindow(shared_from_this());
        return true;

    default:
        return false;
    }
}


void Window::Repaint() {

    Rect dirty_rect;

    RECT win32_rect = { 0 };
    if (GetUpdateRect(handle_, &win32_rect, TRUE)) {
        dirty_rect = Rect::FromRECT(win32_rect);
    }
    else {
        dirty_rect = root_control_->Rect();
    }

    //The update rect must be validated before painting.
    //Because some controls may call NeedRepaint while it is painting,
    //and this may fails if there is an invalidated update rect.
    ValidateRect(handle_, nullptr);

    renderer_.BeginDraw();

    Canvas canvas(renderer_, root_control_->Rect(), dirty_rect);

    //Paint window background color first.
    {
        Canvas::StateGuard state_guard(canvas);
        canvas.SetBrushWithColor(Color::FromRGB(internal::ControlBackgroundColorRGB));
        canvas.DrawRectangle(dirty_rect);
    }

    root_control_->Repaint(canvas, dirty_rect);

    PaintInspectedControl(canvas, dirty_rect);

    if (caret_ != nullptr) {
        const Rect& caret_rect = caret_->GetRect();
        if (caret_rect.HasIntersection(dirty_rect)) {
            caret_->Repaint(canvas);
        }
    }

    try {
        renderer_.EndDraw();
    }
    catch (const Error& error) {
        if (error.Code() == MakeComErrorCode(D2DERR_RECREATE_TARGET)) {
            RecreateRenderer();
        }
    }
}


void Window::PaintInspectedControl(Canvas& canvas, const Rect& dirty_rect) {

    if (!highlight_control_) {
        return;
    }

    auto control_rect = highlight_control_->GetAbsoluteRect();
    if (!control_rect.HasIntersection(dirty_rect)) {
        return;
    }

    auto padding_rect = control_rect;
    padding_rect.Deflate(highlight_control_->GetBorder());

    auto content_rect = padding_rect;
    content_rect.Deflate(highlight_control_->GetPadding());

    auto margin_rect = control_rect;
    margin_rect.Inflate(highlight_control_->GetMargin());

    auto draw_frame = [&canvas](
        const Rect& rect, 
        const Rect excluded_rect,
        std::uint32_t color_rgb) {
    
        auto rect_geometry = canvas.CreateRectangleGeometry(rect);
        auto excluded_geometry = canvas.CreateRectangleGeometry(excluded_rect);

        auto frame_geometry = canvas.CreatePathGeometry();
        auto sink = frame_geometry.Open();
        Geometry::Combine(rect_geometry, excluded_geometry, Geometry::CombineMode::Exclude, sink);
        sink.Close();
        
        auto color = Color::FromRGB(color_rgb);
        color.a /= 2.f;
        canvas.SetBrushWithColor(color);
        canvas.DrawGeometry(frame_geometry);
    };

    Canvas::StateGuard state_guard(canvas);
    canvas.PushClippingRect(dirty_rect);

    //Draw content rect.
    draw_frame(content_rect, Rect{}, internal::InspectedControlContentColor);

    //Draw padding rect.
    draw_frame(padding_rect, content_rect, internal::InspectedControlPaddingColor);

    //Draw border rect.
    draw_frame(control_rect, padding_rect, internal::InspectedControlBorderColor);

    //Draw margin rect.
    draw_frame(margin_rect, control_rect, internal::InspectedControlMarginColor);

    canvas.PopClippingRect();
}


void Window::NeedRepaintRect(const Rect& rect) {

    if (handle_ != nullptr) {
        RECT win32_rect = Align(rect).ToRECT();
        InvalidateRect(handle_, &win32_rect, FALSE);
    }
}


void Window::Resize(UINT width, UINT height) {

    Size size(static_cast<float>(width), static_cast<float>(height));
    if (renderer_ != nullptr) {
        renderer_.Resize(size);
    }
    root_control_->SetRect(Rect(Point(), size));
}


std::optional<HitTestResult> Window::HitTest(const HitTestMessage& message) {

    auto hovered_control = [&]() {
        
        Point mouse_position = message.GetMousePosition();
        std::shared_ptr<Control> current_control = root_control_;
        while (true) {

            auto child = current_control->FindChildAtPosition(mouse_position);
            if (child == nullptr) {
                break;
            }

            auto child_position = child->GetPosition();
            mouse_position.x += child_position.x;
            mouse_position.y += child_position.y;

            current_control = child;
        }
        return current_control;
    }();

    return hovered_control->HitTest(message);
}


bool Window::RedirectMouseWheelMessage(const Message& message) {

    HWND handle = GetCapture();
    if ((handle == nullptr) || (handle == GetHandle())) {
        return false;
    }

    auto capturing_mouse_window = GetWindowFromHandle(GetCapture());
    if (capturing_mouse_window == nullptr) {
        return false;
    }

    PostMessage(capturing_mouse_window->GetHandle(), message.id, message.wparam, message.lparam);
    return true;
}


bool Window::ReceiveMouseMessage(const MouseMessage& message) {

    bool is_capturing_mouse = capturing_mouse_control_ != nullptr;
    auto get_mouse_position_to_capturing_control = [this, &message]() {

        Rect control_rect = capturing_mouse_control_->GetAbsoluteRect();
        Point mouse_position = message.GetMousePosition();

        return Point(
            mouse_position.x - control_rect.position.x,
            mouse_position.y - control_rect.position.y);
    };

    if (message.id == WM_MOUSEMOVE || message.id == WM_NCMOUSEMOVE) {

        if (is_selecting_inspector_control_) {
            HighlightControlAtPosition(message.GetMousePosition());
            return true;
        }

        TrackMouseLeave(message);

        if (is_capturing_mouse) {
            capturing_mouse_control_->RouteHoverMessage(
                get_mouse_position_to_capturing_control(),
                message);
        }
        else {
            root_control_->RouteHoverMessage(message.GetMousePosition(), message);
        }
    }
    else if (message.id == WM_MOUSELEAVE || message.id == WM_NCMOUSELEAVE) {
        OnMouseLeave(message);
    }
    else if (message.id == WM_LBUTTONDOWN || message.id == WM_RBUTTONDOWN) {
        if (is_selecting_inspector_control_) {
            SelectInspectedControl();
            return true;
        }
    }

    if (is_capturing_mouse) {
        return capturing_mouse_control_->RouteMessage(get_mouse_position_to_capturing_control(), message);
    }
    else {
        return root_control_->RouteMessage(message.GetMousePosition(), message);
    }
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


void Window::TrackMouseLeave(const MouseMessage& message) {

    auto is_tracking_mouse = [&]() {

        if (track_mouse_mode_ == TrackMouseMode::ClientArea &&
            message.id == WM_MOUSEMOVE) {
            return true;
        }

        if (track_mouse_mode_ == TrackMouseMode::NonClientArea &&
            message.id == WM_NCMOUSEMOVE) {
            return true;
        }

        return false;
    }();

    if (is_tracking_mouse) {
        return;
    }

    TRACKMOUSEEVENT track_mouse_event_param = { 0 };
    track_mouse_event_param.cbSize = sizeof(track_mouse_event_param);
    track_mouse_event_param.dwFlags = TME_LEAVE;
    if (message.id == WM_NCMOUSEMOVE) {
        track_mouse_event_param.dwFlags |= TME_NONCLIENT;
    }
    track_mouse_event_param.hwndTrack = handle_;

    if (TrackMouseEvent(&track_mouse_event_param)) {
        track_mouse_mode_ = message.id == WM_NCMOUSEMOVE ? TrackMouseMode::NonClientArea : TrackMouseMode::ClientArea;
    }
}


void Window::OnMouseLeave(const MouseMessage& message) {

    bool is_tracking_mouse = [&]() {
    
        if (track_mouse_mode_ == TrackMouseMode::ClientArea &&
            message.id == WM_MOUSELEAVE) {
            return true;
        }

        if (track_mouse_mode_ == TrackMouseMode::NonClientArea &&
            message.id == WM_NCMOUSELEAVE) {
            return true;
        }

        return false;
    }();

    if (is_tracking_mouse) {
        track_mouse_mode_ = TrackMouseMode::None;
        SetHoveredControl(nullptr, {});
    }
}


bool Window::ChangeMouseCursor(const Message& message) {

    bool is_changed = false;

    if (hovered_control_ != nullptr) {
        hovered_control_->ChangeMouseCursor(message, is_changed);
    }

    return is_changed;
}


bool Window::ReceiveCloseMessage() {

    bool can_close = GetCloseHandler()(*this);
    return can_close;
}


void Window::ReceiveDestroyMessage() {

    if (capturing_mouse_control_ != nullptr) {
        capturing_mouse_control_->IsCapturingMouseChanged(false);
        capturing_mouse_control_ = nullptr;
    }

    if (focused_control_ != nullptr) {
        focused_control_->IsFocusedChanged(false);
        focused_control_ = nullptr;
    }

    root_control_->ReleaseRendererResources();

    HWND old_handle = handle_;

    handle_ = nullptr;
    renderer_.Reset();

    OnWindowDestroyed(old_handle);
}


void Window::OnWindowDestroyed(HWND handle) {

    auto event_observer = GetEventObserver<WindowDestroyInfo>(
        GetPropertyMap(),
        kDestroyEventPropertyName);

    if (event_observer) {

        WindowDestroyInfo event_info;
        event_info.window = shared_from_this();
        event_observer->OnNext(event_info);
    }
}


void Window::SetHoveredControl(
    const std::shared_ptr<Control>& hovered_control, 
    const MouseMessage& message) {

    if (hovered_control_ == hovered_control) {
        return;
    }

    if (hovered_control != nullptr) {

        //The hovering control must be contained in this window
        if (hovered_control->GetWindow().get() != this) {
            return;
        }
    }

    if (hovered_control_ != nullptr) {
        hovered_control_->IsHoveredChanged(false);
    }

    hovered_control_ = hovered_control;

    if (hovered_control_ != nullptr) {

        //Window finds the hovered control when received WM_MOUSEMOVE,
        //but WM_SETCURSOR is always received before WM_MOUSEMOVE, in such
        //case the hovered control cannot change the cursor promptly. So 
        //here, a simulated WM_SETCURSOR is posted to give a change to 
        //change the cursor immediately.
        PostMessage(
            GetHandle(),
            WM_SETCURSOR,
            reinterpret_cast<WPARAM>(GetHandle()),
            MAKELPARAM(message.GetHitTestResult(), message.id));

        hovered_control_->IsHoveredChanged(true);
    }
}


void Window::SetCaptureMouseControl(const std::shared_ptr<Control>& capture_control, bool is_releasing) {

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
    
    auto previous_control = capturing_mouse_control_;
    if (previous_control == control) {
        return;
    }

    //The control must be contained in this window.
    if (control->GetWindow().get() != this) {
        return;
    }

    if (previous_control != nullptr) {
        previous_control->IsCapturingMouseChanged(false);
    }
    else {
        SetCapture(GetHandle());
    }

    capturing_mouse_control_ = control;
    capturing_mouse_control_->IsCapturingMouseChanged(true);

    OnCapturingMouseControlChanged(previous_control);
}


void Window::ReleaseMouseWithControl(const std::shared_ptr<Control>& control) {

    if (capturing_mouse_control_ != control) {
        return;
    }

    //ReleaseCapture sends a WM_CAPTURECAHNGED message to the window, 
    //in which message handler will set capturing_mouse_control_ to nullptr.
    ReleaseCapture();
}


void Window::SetFocusedControl(const std::shared_ptr<Control>& new_focused_control) {

    auto previous_focused_control = focused_control_;
    if (previous_focused_control == new_focused_control) {
        return;
    }

    if (new_focused_control != nullptr) {

        if (!new_focused_control->IsEnabled()) {
            return;
        }

        //The focused control must be contained in this window
        if (new_focused_control->GetWindow().get() != this) {
            return;
        }
    }

    if (previous_focused_control != nullptr) {
        previous_focused_control->IsFocusedChanged(false);
    }

    focused_control_ = new_focused_control;

    if (new_focused_control != nullptr) {
        new_focused_control->IsFocusedChanged(true);
    }

    OnFocusedControlChanged(previous_focused_control);
}


std::shared_ptr<Window> Window::GetOwner() const {

    auto owner = GetPropertyMap().TryGetProperty<std::weak_ptr<Window>>(kOwnerPropertyName);
    if (owner) {
        return owner->lock();
    }
    else {
        return nullptr;
    }
}


void Window::SetOwner(const std::shared_ptr<Window>& owner) {

    if (IsClosed()) {

        std::weak_ptr<Window> weak_owner = owner;
        GetPropertyMap().SetProperty(kOwnerPropertyName, weak_owner);
    }
}


Window::InitialRectStyle Window::GetInitialRectStyle() const {

    auto style = GetPropertyMap().TryGetProperty<InitialRectStyle>(kInitialRectStylePropertyName);
    if (style != nullptr) {
        return *style;
    }
    else {
        return InitialRectStyle::CenterInScreen;
    }
}


void Window::SetInitialRectStyle(InitialRectStyle initial_rect_style) {

    GetPropertyMap().SetProperty(kInitialRectStylePropertyName, initial_rect_style);
}


Rect Window::GetRect() const {

    if (IsClosed()) {
        return rect_;
    }
    else {

        RECT rect = { 0 };
        GetWindowRect(handle_, &rect);
        return Rect::FromRECT(rect);
    }
}


void Window::SetRect(const Rect& rect) {

    rect_ = rect;

    if (! IsClosed()) {

        SetWindowPos(
            handle_,
            nullptr,
            static_cast<int>(rect_.position.x),
            static_cast<int>(rect_.position.y),
            static_cast<int>(rect_.size.width),
            static_cast<int>(rect_.size.height),
            SWP_NOZORDER | SWP_NOACTIVATE);
    }
}


void Window::SetClientSize(const Size& size) {

    auto adjusted_rect = Rect{ Point{}, size }.ToRECT();

    DWORD style{};
    DWORD extra_style{};
    GetHandleStyles(style, extra_style);

    BOOL is_succeeded = AdjustWindowRectEx(&adjusted_rect, style, FALSE, extra_style);
    if (!is_succeeded) {
        ZAF_THROW_SYSTEM_ERROR(GetLastError());
    }

    Rect new_rect;
    new_rect.position = GetPosition();
    new_rect.size = Size{
        static_cast<float>(adjusted_rect.right - adjusted_rect.left),
        static_cast<float>(adjusted_rect.bottom - adjusted_rect.top) 
    };

    SetRect(new_rect);
}


float Window::GetMinWidth() const {

    auto width = GetPropertyMap().TryGetProperty<float>(property::MinWidth);
    if (width != nullptr) {
        return *width;
    }
    return static_cast<float>(GetSystemMetrics(SM_CXMINTRACK));
}


void Window::SetMinWidth(float min_width) {

    GetPropertyMap().SetProperty(property::MinWidth, min_width);

    if (GetMaxWidth() < min_width) {
        SetMaxWidth(min_width);
    }

    if (GetWidth() < min_width) {
        SetWidth(min_width);
    }
}


float Window::GetMaxWidth() const {

    auto width = GetPropertyMap().TryGetProperty<float>(property::MaxWidth);
    if (width != nullptr) {
        return *width;
    }
    return static_cast<float>(GetSystemMetrics(SM_CXMAXTRACK));
}


void Window::SetMaxWidth(float max_width) {

    GetPropertyMap().SetProperty(property::MaxWidth, max_width);

    if (GetMinWidth() > max_width) {
        SetMinWidth(max_width);
    }

    if (GetWidth() > max_width) {
        SetWidth(max_width);
    }
}


float Window::GetMinHeight() const {

    auto height = GetPropertyMap().TryGetProperty<float>(property::MinHeight);
    if (height != nullptr) {
        return *height;
    }
    return static_cast<float>(GetSystemMetrics(SM_CYMINTRACK));
}


void Window::SetMinHeight(float min_height) {

    GetPropertyMap().SetProperty(property::MinHeight, min_height);

    if (GetMaxHeight() < min_height) {
        SetMaxHeight(min_height);
    }

    if (GetHeight() < min_height) {
        SetHeight(min_height);
    }
}


float Window::GetMaxHeight() const {

    auto height = GetPropertyMap().TryGetProperty<float>(property::MaxHeight);
    if (height != nullptr) {
        return *height;
    }
    return static_cast<float>(GetSystemMetrics(SM_CYMAXTRACK));
}


void Window::SetMaxHeight(float max_height) {

    GetPropertyMap().SetProperty(property::MaxHeight, max_height);

    if (GetMinHeight() > max_height) {
        SetMinHeight(max_height);
    }

    if (GetHeight() > max_height) {
        SetHeight(max_height);
    }
}


Rect Window::GetClientRect() const {

    RECT rect = { 0 };
    ::GetClientRect(handle_, &rect);
    return Rect::FromRECT(rect);
}


Window::ActivateOption Window::GetActivateOption() const {

    auto option = GetPropertyMap().TryGetProperty<ActivateOption>(kActivateOptionPropertyName);
    if (option != nullptr) {
        return *option;
    }
    else {
        return ActivateOption::None;
    }
}

void Window::SetActivateOption(ActivateOption option) {

    if (IsClosed()) {
        GetPropertyMap().SetProperty(kActivateOptionPropertyName, option);
    }
}


bool Window::IsPopup() const {
    auto value = GetPropertyMap().TryGetProperty<bool>(kIsPopupPropertyName);
    return value ? *value : false;
}

void Window::SetIsPopup(bool is_popup) {

    if (IsClosed()) {
        GetPropertyMap().SetProperty(kIsPopupPropertyName, is_popup);
        ReviseHasTitleBar();
    }
}


bool Window::HasBorder() const {
    auto value = GetPropertyMap().TryGetProperty<bool>(kHasBorderPropertyName);
    return value ? *value : true;
}

void Window::SetHasBorder(bool has_border) {

    if (IsClosed()) {
        GetPropertyMap().SetProperty(kHasBorderPropertyName, has_border);
        ReviseHasTitleBar();
    }
}


bool Window::HasTitleBar() const {
    auto value = GetPropertyMap().TryGetProperty<bool>(kHasTitleBarPropertyName);
    return value ? *value : true;
}


void Window::SetHasTitleBar(bool has_title_bar) {

    if (!IsPopup() && HasBorder()) {
        return;
    }

    GetPropertyMap().SetProperty(kHasTitleBarPropertyName, has_title_bar);

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
    auto value = GetPropertyMap().TryGetProperty<bool>(kIsSizablePropertyName);
    return value ? *value : true;
}

void Window::SetIsSizable(bool is_sizable) {

    GetPropertyMap().SetProperty(kIsSizablePropertyName, is_sizable);

    if (HasBorder()) {
        SetStyleToHandle(WS_SIZEBOX, is_sizable, false);
    }
}


bool Window::HasSystemMenu() const {
    auto value = GetPropertyMap().TryGetProperty<bool>(kHasSystemMenuPropertyName);
    return value ? *value : true;
}

void Window::SetHasSystemMenu(bool has_system_menu) {
    SetStyleProperty(kHasSystemMenuPropertyName, WS_SYSMENU, has_system_menu, false);
}


bool Window::CanMinimize() const {
    auto value = GetPropertyMap().TryGetProperty<bool>(kCanMinimizePropertyName);
    return value ? *value : true;
}

void Window::SetCanMinimize(bool can_minimize) {
    SetStyleProperty(kCanMinimizePropertyName, WS_MINIMIZEBOX, can_minimize, false);
}


bool Window::CanMaximize() const {
    auto value = GetPropertyMap().TryGetProperty<bool>(kCanMaximizePropertyName);
    return value ? *value : true;
}

void Window::SetCanMaximize(bool has_maximize_button) {
    SetStyleProperty(kCanMaximizePropertyName, WS_MAXIMIZEBOX, has_maximize_button, false);
}


bool Window::IsToolWindow() const {

    auto is_set = GetPropertyMap().TryGetProperty<bool>(kIsToolWindowPropertyName);
    if (is_set != nullptr) {
        return *is_set;
    }
    return false;
}

void Window::SetIsToolWindow(bool is_tool_window) {
    SetStyleProperty(kIsToolWindowPropertyName, WS_EX_TOOLWINDOW, is_tool_window, true);
}


void Window::SetStyleProperty(
    const std::wstring& property_name,
    DWORD style_value,
    bool is_set,
    bool is_extra_style) {

    GetPropertyMap().SetProperty(property_name, is_set);

    if (! IsClosed()) {
        SetStyleToHandle(style_value, is_set, is_extra_style);
    }
}


void Window::SetStyleToHandle(DWORD style_value, bool is_set, bool is_extra_style) {

    DWORD category = is_extra_style ? GWL_EXSTYLE : GWL_STYLE;

    DWORD style = GetWindowLong(GetHandle(), category);
    if (is_set) {
        style |= style_value;
    }
    else {
        style &= ~style_value;
    }
    SetWindowLong(GetHandle(), category, style);
}


std::wstring Window::GetTitle() const {

    if (IsClosed()) {

        auto title = GetPropertyMap().TryGetProperty<std::wstring>(kTitlePropertyName);
        if (title != nullptr) {
            return *title;
        }
        else {
            return std::wstring();
        }
    }
    else {

        int title_length = GetWindowTextLength(handle_);
        std::vector<wchar_t> buffer(title_length + 1);
        GetWindowText(handle_, buffer.data(), buffer.size());
        return buffer.data();
    }
}


void Window::SetTitle(const std::wstring& title) {

    GetPropertyMap().SetProperty(kTitlePropertyName, title);

    if (! IsClosed()) {
        SetWindowText(handle_, title.c_str());
    }
}


void Window::SetRootControl(const std::shared_ptr<Control>& control) {
    InitializeRootControl(control != nullptr ? control : Create<Control>());
}


void Window::InitializeRootControl(const std::shared_ptr<Control>& control) {

    if (root_control_ == control) {
        return;
    }

    auto previous_root_control = root_control_;
    if (previous_root_control != nullptr) {
        previous_root_control->ReleaseRendererResources();
        previous_root_control->SetWindow(nullptr);
    }

    root_control_ = control;
    root_control_->SetWindow(shared_from_this());

    if (!IsClosed()) {
        RECT client_rect{};
        ::GetClientRect(handle_, &client_rect);
        root_control_->SetRect(Rect::FromRECT(client_rect));
    }

    OnRootControlChanged(previous_root_control);
}


const std::shared_ptr<Caret>& Window::GetCaret() {

    if (caret_ == nullptr) {
        caret_ = std::make_shared<Caret>();
        caret_->SetWindow(shared_from_this());
    }
    return caret_;
}


const Window::CloseHandler Window::GetCloseHandler() const {

    auto handler = GetPropertyMap().TryGetProperty<CloseHandler>(kCloseHandlerPropertyName);
    if ((handler != nullptr) && (*handler != nullptr)) {
        return *handler;
    }
    else {

        return [](const Window&) {
            return true;
        };
    }
}


void Window::SetCloseHandler(const CloseHandler& handler) {
    GetPropertyMap().SetProperty(kCloseHandlerPropertyName, handler);
}


Observable<WindowDestroyInfo> Window::DestroyEvent() {
    return GetEventObservable<WindowDestroyInfo>(GetPropertyMap(), kDestroyEventPropertyName);
}


const Point Window::GetMousePosition() const {

    POINT cursor_point = { 0 };
    GetCursorPos(&cursor_point);
    ScreenToClient(handle_, &cursor_point);

    return Point(static_cast<float>(cursor_point.x), static_cast<float>(cursor_point.y));
}


void Window::Show() {

    CheckCreateWindowHandle();

    auto activate_option = GetActivateOption();
    bool no_activate = (activate_option & ActivateOption::NoActivate) == ActivateOption::NoActivate;
    ShowWindow(handle_, no_activate ? SW_SHOWNA : SW_SHOW);

    OnWindowShown();
}


void Window::Hide() {
    ShowWindow(handle_, SW_HIDE);
}


void Window::Maximize() {
    ShowWindow(handle_, SW_MAXIMIZE);
}


void Window::Minimize() {
    ShowWindow(handle_, SW_MINIMIZE);
}


void Window::Restore() {
    ShowWindow(handle_, SW_RESTORE);
}


void Window::Close() {
    SendMessage(GetHandle(), WM_CLOSE, 0, 0);
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

    if (highlight_control->GetWindow().get() != this) {
        return;
    }

    //Repaint the rect of previous highlight control.
    if (highlight_control_) {
        NeedRepaintRect(highlight_control_->GetAbsoluteRect());
    }

    highlight_control_ = highlight_control;

    //Repaint the rect of new highlight control.
    NeedRepaintRect(highlight_control_->GetAbsoluteRect());
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