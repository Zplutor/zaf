#include <zaf/window/window.h>
#include <Windowsx.h>
#include <zaf/application.h>
#include <zaf/base/error/system_error.h>
#include <zaf/base/event_utility.h>
#include <zaf/creation.h>
#include <zaf/graphic/alignment.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/dpi.h>
#include <zaf/graphic/graphic_factory.h>
#include <zaf/internal/tab_stop_utility.h>
#include <zaf/internal/theme.h>
#include <zaf/object/parsing/object_parser.h>
#include <zaf/object/parsing/xaml_node_parse_helper.h>
#include <zaf/object/type_definition.h>
#include <zaf/serialization/properties.h>
#include <zaf/window/caret.h>
#include <zaf/window/inspector/inspector_window.h>
#include <zaf/window/tooltip_window.h>
#include <zaf/window/message/hit_test_message.h>
#include <zaf/window/message/keyboard_message.h>
#include <zaf/window/message/message.h>
#include <zaf/window/message/mouse_message.h>

namespace zaf {
namespace {

const wchar_t* const kDefaultWindowClassName = L"ZafDefaultWindowClass";

constexpr const wchar_t* const kActivateOptionPropertyName = L"ActivateOption";
constexpr const wchar_t* const kCanMaximizePropertyName = L"CanMaximize";
constexpr const wchar_t* const kCanMinimizePropertyName = L"CanMinimize";
constexpr const wchar_t* const kCloseEventPropertyName = L"CloseEvent";
constexpr const wchar_t* const kDestroyEventPropertyName = L"DestroyEvent";
constexpr const wchar_t* const kHasBorderPropertyName = L"HasBorder";
constexpr const wchar_t* const kHasSystemMenuPropertyName = L"HasSystemMenu";
constexpr const wchar_t* const kHasTitleBarPropertyName = L"HasTitleBar";
constexpr const wchar_t* const kInitialRectStylePropertyName = L"InitialRectStyle";
constexpr const wchar_t* const kIsPopupPropertyName = L"IsPopup";
constexpr const wchar_t* const kIsSizablePropertyName = L"IsSizable";
constexpr const wchar_t* const kIsToolWindowPropertyName = L"IsToolWindow";
constexpr const wchar_t* const kIsTopmostPropertyName = L"IsTopmost";
constexpr const wchar_t* const kOwnerPropertyName = L"Owner";
constexpr const wchar_t* const kReceiveMessageEventPropertyName = L"ReceiveMessageEvent";
constexpr const wchar_t* const kTitlePropertyName = L"Title";


zaf::Rect ToAlignedPixelsRect(const zaf::Rect& rect, float dpi) {
    return Align(FromDIPs(rect, dpi));
}


class WindowParser : public ObjectParser {
public:
    void ParseFromAttribute(const std::wstring& attribute_value, Object& object) override {
        //Nothing to do.
    }

    void ParseFromNode(const XamlNode& node, Object& reflection_object) override {

        auto& window = dynamic_cast<Window&>(reflection_object);
        ParseControls(node, window);
    }

private:
    void ParseControls(const XamlNode& node, Window& window) {

        std::vector<std::shared_ptr<Control>> controls;

        for (const auto& each_node : node.GetContentNodes()) {

            auto control = internal::CreateObjectFromNode<Control>(each_node);
            if (control == nullptr) {
                continue;
            }

            controls.push_back(control);
        }

        if (!controls.empty()) {
            window.RootControl()->AddChildren(controls);
        }
    }
};

}

ZAF_DEFINE_TYPE(Window)
ZAF_DEFINE_TYPE_PARSER(WindowParser)
ZAF_DEFINE_TYPE_PROPERTY(Owner)
ZAF_DEFINE_TYPE_PROPERTY(InitialRectStyle)
ZAF_DEFINE_TYPE_PROPERTY(Rect)
ZAF_DEFINE_TYPE_PROPERTY(Position)
ZAF_DEFINE_TYPE_PROPERTY(Size)
ZAF_DEFINE_TYPE_PROPERTY(ContentSize)
ZAF_DEFINE_TYPE_PROPERTY(ContentWidth)
ZAF_DEFINE_TYPE_PROPERTY(ContentHeight)
ZAF_DEFINE_TYPE_PROPERTY(MinSize)
ZAF_DEFINE_TYPE_PROPERTY(MaxSize)
ZAF_DEFINE_TYPE_PROPERTY(Width)
ZAF_DEFINE_TYPE_PROPERTY(MinWidth)
ZAF_DEFINE_TYPE_PROPERTY(MaxWidth)
ZAF_DEFINE_TYPE_PROPERTY(Height)
ZAF_DEFINE_TYPE_PROPERTY(MinHeight)
ZAF_DEFINE_TYPE_PROPERTY(MaxHeight)
ZAF_DEFINE_TYPE_PROPERTY(ActivateOption)
ZAF_DEFINE_TYPE_PROPERTY(IsPopup)
ZAF_DEFINE_TYPE_PROPERTY(HasBorder)
ZAF_DEFINE_TYPE_PROPERTY(HasTitleBar)
ZAF_DEFINE_TYPE_PROPERTY(IsSizable)
ZAF_DEFINE_TYPE_PROPERTY(HasSystemMenu)
ZAF_DEFINE_TYPE_PROPERTY(CanMinimize)
ZAF_DEFINE_TYPE_PROPERTY(CanMaximize)
ZAF_DEFINE_TYPE_PROPERTY(IsToolWindow)
ZAF_DEFINE_TYPE_PROPERTY(IsTopmost)
ZAF_DEFINE_TYPE_PROPERTY(Title)
ZAF_DEFINE_TYPE_PROPERTY_DYNAMIC(RootControl)
ZAF_DEFINE_TYPE_PROPERTY(CapturingMouseControl)
ZAF_DEFINE_TYPE_PROPERTY(MouseOverControl)
ZAF_DEFINE_TYPE_PROPERTY(FocusedControl)
ZAF_DEFINE_TYPE_PROPERTY(IsVisible)
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

        LRESULT result = 0;
        if (window->ReceiveMessageEntrance(Message{ hwnd, message_id, wparam, lparam }, result)) {
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
    Destroy();
}


void Window::Initialize() {

    __super::Initialize();

    InitializeRootControl(Create<Control>());
}


void Window::CreateWindowHandle() {

    //Revise HasTitleBar property first.
    ReviseHasTitleBar();

    auto owner = Owner();

    DWORD style = 0;
    DWORD extra_style = 0;
    GetHandleStyles(style, extra_style);

    handle_ = CreateWindowEx(
        extra_style,
        kDefaultWindowClassName,
        Title().c_str(),
        style,
        0,
        0,
        0,
        0,
        owner == nullptr ? nullptr : owner->Handle(),
        nullptr,
        nullptr,
        this);

    auto dpi = static_cast<float>(GetDpiForWindow(handle_));

    RECT window_rect{};
    GetWindowRect(handle_, &window_rect);
    rect_ = ToDIPs(Rect::FromRECT(window_rect), dpi);

    RECT client_rect{};
    GetClientRect(handle_, &client_rect);
    root_control_->SetRect(ToDIPs(Rect::FromRECT(client_rect), dpi));

    CreateRenderer();
    Application::Instance().RegisterWindow(shared_from_this());

    OnWindowCreated();
}


void Window::ReceiveCreateMessage(HWND handle) {

    auto dpi = static_cast<float>(GetDpiForWindow(handle));
    auto initial_rect = GetInitialRect(dpi);
    auto rect_in_pixels = ToAlignedPixelsRect(initial_rect, dpi);

    SetWindowPos(
        handle,
        nullptr,
        static_cast<int>(rect_in_pixels.position.x),
        static_cast<int>(rect_in_pixels.position.y),
        static_cast<int>(rect_in_pixels.size.width),
        static_cast<int>(rect_in_pixels.size.height),
        SWP_NOZORDER | SWP_NOACTIVATE);
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

    renderer_ = GetGraphicFactory().CreateWindowRenderer(handle_);
}


void Window::RecreateRenderer() {

    root_control_->ReleaseRendererResources();
    CreateRenderer();
}


void Window::CheckCreateWindowHandle() {

    if (!Handle()) {
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

    if (IsTopmost()) {
        handle_extra_style |= WS_EX_TOPMOST;
    }

    auto activate_option = ActivateOption();
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

    if (message.Inner().id != WM_KEYDOWN || message.VirtualKey() != VK_TAB) {
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
    if (message.Inner().id != WM_KEYDOWN) {
        return false;
    }

    if (message.VirtualKey() != VK_F12) {
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


bool Window::ReceiveMessageEntrance(const Message& message, LRESULT& result) {

    bool is_handled = ReceiveMessage(message, result);
    RaiseReceiveMessageEvent(message, result);
    return is_handled;
}


void Window::RaiseReceiveMessageEvent(const Message& message, LRESULT result) {

    if (message.id == WM_NCDESTROY) {
        return;
    }

    auto event_observer = GetEventObserver<WindowReceiveMessageInfo>(
        GetPropertyMap(),
        kReceiveMessageEventPropertyName);

    if (!event_observer) {
        return;
    }

    WindowReceiveMessageInfo event_info(shared_from_this(), message, result);
    event_observer->OnNext(event_info);
}


bool Window::ReceiveMessage(const Message& message, LRESULT& result) {

    auto is_customized_style = [this]() {
        return !IsPopup() && !HasBorder();
    };

    switch (message.id) {
    case WM_CREATE:
        ReceiveCreateMessage(message.hwnd);
        return true;

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
        min_max_info->ptMinTrackSize.x = static_cast<LONG>(MinWidth());
        min_max_info->ptMinTrackSize.y = static_cast<LONG>(MinHeight());
        min_max_info->ptMaxTrackSize.x = static_cast<LONG>(MaxWidth());
        min_max_info->ptMaxTrackSize.y = static_cast<LONG>(MaxHeight());
        result = 0;
        return true;
    }

    case WM_SIZE:
        Resize(LOWORD(message.lparam), HIWORD(message.lparam));
        return true;

    case WM_MOVE:
        ReceiveMoveMessage();
        return true;

    case WM_MOUSEACTIVATE: {
        auto activate_option = ActivateOption();
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
        auto hit_test_result = HitTest(HitTestMessage{ message });
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
        return ReceiveMouseMessage(MouseWheelMessage{ message });

    case WM_MOUSEMOVE:
    case WM_NCMOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_NCLBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_NCLBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
        return ReceiveMouseMessage(MouseMessage{ message });

    case WM_MOUSEHOVER:
    case WM_NCMOUSEHOVER:
        OnMouseHover(message);
        return true;

    case WM_MOUSELEAVE:
    case WM_NCMOUSELEAVE:
        OnMouseLeave(message);
        return true;

    case WM_KEYDOWN: 
        if (focused_control_ != nullptr) {
            return focused_control_->OnKeyDown(KeyMessage{ message });
        }
        return false;

    case WM_KEYUP:
        if (focused_control_ != nullptr) {
            return focused_control_->OnKeyUp(KeyMessage{ message });
        }
        return false;

    case WM_CHAR:
        if (focused_control_ != nullptr) {
            return focused_control_->OnCharInput(CharMessage{ message });
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
        const zaf::Rect& caret_rect = caret_->GetRect();
        if (caret_rect.HasIntersection(dirty_rect)) {
            caret_->Repaint(canvas);
        }
    }

    try {
        renderer_.EndDraw();
    }
    catch (const Error& error) {
        if (error.Code() == MakeCOMErrorCode(D2DERR_RECREATE_TARGET)) {
            RecreateRenderer();
        }
    }
}


void Window::PaintInspectedControl(Canvas& canvas, const zaf::Rect& dirty_rect) {

    if (!highlight_control_) {
        return;
    }

    auto control_rect = highlight_control_->AbsoluteRect();
    if (!control_rect.HasIntersection(dirty_rect)) {
        return;
    }

    auto padding_rect = control_rect;
    padding_rect.Deflate(highlight_control_->Border());

    auto content_rect = padding_rect;
    content_rect.Deflate(highlight_control_->Padding());

    auto margin_rect = control_rect;
    margin_rect.Inflate(highlight_control_->Margin());

    auto draw_frame = [&canvas](
        const zaf::Rect& rect,
        const zaf::Rect excluded_rect,
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
    draw_frame(content_rect, zaf::Rect{}, internal::InspectedControlContentColor);

    //Draw padding rect.
    draw_frame(padding_rect, content_rect, internal::InspectedControlPaddingColor);

    //Draw border rect.
    draw_frame(control_rect, padding_rect, internal::InspectedControlBorderColor);

    //Draw margin rect.
    draw_frame(margin_rect, control_rect, internal::InspectedControlMarginColor);

    canvas.PopClippingRect();
}


void Window::NeedRepaintRect(const zaf::Rect& rect) {

    if (handle_ != nullptr) {
        RECT win32_rect = ToAlignedPixelsRect(rect, GetDPI()).ToRECT();
        InvalidateRect(handle_, &win32_rect, FALSE);
    }
}


void Window::Resize(UINT width, UINT height) {

    zaf::Size size(static_cast<float>(width), static_cast<float>(height));
    if (renderer_ != nullptr) {
        renderer_.Resize(size);
    }

    zaf::Rect root_control_rect{ Point(), ToDIPs(size, GetDPI()) };
    root_control_->SetRect(root_control_rect);

    UpdateWindowRect();
}


void Window::ReceiveMoveMessage() {

    UpdateWindowRect();
}


void Window::UpdateWindowRect() {

    RECT rect{};
    GetWindowRect(handle_, &rect);
    rect_ = ToDIPs(Rect::FromRECT(rect), GetDPI());
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

    PostMessage(capturing_mouse_window->Handle(), message.id, message.wparam, message.lparam);
    return true;
}


bool Window::ReceiveMouseMessage(const MouseMessage& message) {

    bool is_capturing_mouse = capturing_mouse_control_ != nullptr;
    auto get_mouse_position_to_capturing_control = [this, &message]() {

        zaf::Rect control_rect = capturing_mouse_control_->AbsoluteRect();
        Point mouse_position = message.MousePosition();

        return Point(
            mouse_position.x - control_rect.position.x,
            mouse_position.y - control_rect.position.y);
    };

    if (message.ID() == WM_MOUSEMOVE || message.ID() == WM_NCMOUSEMOVE) {

        if (is_selecting_inspector_control_) {
            HighlightControlAtPosition(message.MousePosition());
            return true;
        }

        TrackMouseByMouseMove(message);

        if (is_capturing_mouse) {
            capturing_mouse_control_->RouteMouseMoveMessage(
                get_mouse_position_to_capturing_control(),
                message);
        }
        else {
            root_control_->RouteMouseMoveMessage(message.MousePosition(), message);
        }
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

    if (is_capturing_mouse) {

        return capturing_mouse_control_->RouteMessage(
            get_mouse_position_to_capturing_control(), 
            message);
    }
    else {
        return root_control_->RouteMessage(message.MousePosition(), message);
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

    bool is_tracking_mouse = [&]() {

        if (track_mouse_mode_ == TrackMouseMode::ClientArea &&
            message.id == WM_MOUSEHOVER) {
            return true;
        }

        if (track_mouse_mode_ == TrackMouseMode::NonClientArea &&
            message.id == WM_NCMOUSEHOVER) {
            return true;
        }

        return false;
    }();

    if (!is_tracking_mouse) {
        return;
    }

    if (mouse_over_control_) {
        mouse_over_control_->HandleMouveHover();
    }

    TryToShowTooltipWindow();
}


void Window::OnMouseLeave(const Message& message) {

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
        SetMouseOverControl(nullptr, MouseMessage{ Message{} });
    }
}


bool Window::ChangeMouseCursor(const Message& message) {

    bool is_changed = false;

    if (mouse_over_control_ != nullptr) {
        mouse_over_control_->ChangeMouseCursor(message, is_changed);
    }

    return is_changed;
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


bool Window::ReceiveCloseMessage() {

    bool can_close{ true };

    auto close_event = GetEventObserver<WindowCloseInfo>(GetPropertyMap(), kCloseEventPropertyName);
    if (close_event) {

        WindowCloseInfo event_info(shared_from_this());
        close_event->OnNext(event_info);

        can_close = event_info.CanClose();
    }

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
    tooltip_window_.reset();

    OnWindowDestroyed(old_handle);
}


void Window::OnWindowDestroyed(HWND handle) {

    auto event_observer = GetEventObserver<WindowDestroyInfo>(
        GetPropertyMap(),
        kDestroyEventPropertyName);

    if (event_observer) {

        WindowDestroyInfo event_info(shared_from_this());
        event_observer->OnNext(event_info);
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
        old_control->IsMouseOverChanged(false);
    }

    if (mouse_over_control_ != nullptr) {

        //Window finds the mouse over control when received WM_MOUSEMOVE,
        //but WM_SETCURSOR is always received before WM_MOUSEMOVE, in such
        //case the mouse over control cannot change the cursor promptly. So 
        //here, a simulated WM_SETCURSOR is posted to give a change to 
        //change the cursor immediately.
        PostMessage(
            Handle(),
            WM_SETCURSOR,
            reinterpret_cast<WPARAM>(Handle()),
            MAKELPARAM(message.HitTestResult(), message.ID()));

        mouse_over_control_->IsMouseOverChanged(true);

        //Track mouse again to generate next mouse hover message.
        //TODO: Find out whether the new control is in non-client area.
        TrackMouse(false);
    }

    HideTooltipWindow();
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
    if (control->Window().get() != this) {
        return;
    }

    if (previous_control != nullptr) {
        previous_control->IsCapturingMouseChanged(false);
    }
    else {
        SetCapture(Handle());
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
        if (new_focused_control->Window().get() != this) {
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


std::shared_ptr<Window> Window::Owner() const {

    auto owner = GetPropertyMap().TryGetProperty<std::weak_ptr<Window>>(kOwnerPropertyName);
    if (owner) {
        return owner->lock();
    }
    else {
        return nullptr;
    }
}


void Window::SetOwner(const std::shared_ptr<Window>& owner) {

    if (!Handle()) {

        std::weak_ptr<Window> weak_owner = owner;
        GetPropertyMap().SetProperty(kOwnerPropertyName, weak_owner);
    }
}


InitialRectStyle Window::InitialRectStyle() const {

    auto style = 
        GetPropertyMap().TryGetProperty<zaf::InitialRectStyle>(kInitialRectStylePropertyName);

    if (style != nullptr) {
        return *style;
    }
    else {
        return InitialRectStyle::CenterInScreen;
    }
}


void Window::SetInitialRectStyle(zaf::InitialRectStyle initial_rect_style) {

    GetPropertyMap().SetProperty(kInitialRectStylePropertyName, initial_rect_style);
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

        auto new_rect = ToAlignedPixelsRect(rect, GetDPI());

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


zaf::Size Window::ContentSize() const {

    if (Handle()) {

        RECT client_rect{};
        ::GetClientRect(handle_, &client_rect);
        return ToDIPs(Rect::FromRECT(client_rect).size, GetDPI());
    }
    else {

        auto adjusted_size = AdjustContentSizeToWindowSize(zaf::Size{});

        auto result = Size();
        result.width -= adjusted_size.width;
        result.height -= adjusted_size.height;
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
    auto rounded_rect = ToAlignedPixelsRect(rect, dpi);
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
        ZAF_THROW_SYSTEM_ERROR(GetLastError());
    }

    auto result = zaf::Size{
        static_cast<float>(adjusted_rect.right - adjusted_rect.left),
        static_cast<float>(adjusted_rect.bottom - adjusted_rect.top)
    };

    result = ToDIPs(result, dpi);
    return result;
}


float Window::MinWidth() const {

    auto width = GetPropertyMap().TryGetProperty<float>(property::MinWidth);
    if (width != nullptr) {
        return *width;
    }
    return static_cast<float>(GetSystemMetrics(SM_CXMINTRACK));
}


void Window::SetMinWidth(float min_width) {

    GetPropertyMap().SetProperty(property::MinWidth, min_width);

    if (MaxWidth() < min_width) {
        SetMaxWidth(min_width);
    }

    if (Width() < min_width) {
        SetWidth(min_width);
    }
}


float Window::MaxWidth() const {

    auto width = GetPropertyMap().TryGetProperty<float>(property::MaxWidth);
    if (width != nullptr) {
        return *width;
    }
    return static_cast<float>(GetSystemMetrics(SM_CXMAXTRACK));
}


void Window::SetMaxWidth(float max_width) {

    GetPropertyMap().SetProperty(property::MaxWidth, max_width);

    if (MinWidth() > max_width) {
        SetMinWidth(max_width);
    }

    if (Width() > max_width) {
        SetWidth(max_width);
    }
}


float Window::MinHeight() const {

    auto height = GetPropertyMap().TryGetProperty<float>(property::MinHeight);
    if (height != nullptr) {
        return *height;
    }
    return static_cast<float>(GetSystemMetrics(SM_CYMINTRACK));
}


void Window::SetMinHeight(float min_height) {

    GetPropertyMap().SetProperty(property::MinHeight, min_height);

    if (MaxHeight() < min_height) {
        SetMaxHeight(min_height);
    }

    if (Height() < min_height) {
        SetHeight(min_height);
    }
}


float Window::MaxHeight() const {

    auto height = GetPropertyMap().TryGetProperty<float>(property::MaxHeight);
    if (height != nullptr) {
        return *height;
    }
    return static_cast<float>(GetSystemMetrics(SM_CYMAXTRACK));
}


void Window::SetMaxHeight(float max_height) {

    GetPropertyMap().SetProperty(property::MaxHeight, max_height);

    if (MinHeight() > max_height) {
        SetMinHeight(max_height);
    }

    if (Height() > max_height) {
        SetHeight(max_height);
    }
}


ActivateOption Window::ActivateOption() const {

    auto option = GetPropertyMap().TryGetProperty<zaf::ActivateOption>(kActivateOptionPropertyName);
    if (option != nullptr) {
        return *option;
    }
    else {
        return ActivateOption::None;
    }
}

void Window::SetActivateOption(zaf::ActivateOption option) {

    if (!Handle()) {
        GetPropertyMap().SetProperty(kActivateOptionPropertyName, option);
    }
}


bool Window::IsPopup() const {
    auto value = GetPropertyMap().TryGetProperty<bool>(kIsPopupPropertyName);
    return value ? *value : false;
}

void Window::SetIsPopup(bool is_popup) {

    if (!Handle()) {
        GetPropertyMap().SetProperty(kIsPopupPropertyName, is_popup);
        ReviseHasTitleBar();
    }
}


bool Window::HasBorder() const {
    auto value = GetPropertyMap().TryGetProperty<bool>(kHasBorderPropertyName);
    return value ? *value : true;
}

void Window::SetHasBorder(bool has_border) {

    if (!Handle()) {
        GetPropertyMap().SetProperty(kHasBorderPropertyName, has_border);
        ReviseHasTitleBar();
    }
}


bool Window::HasTitleBar() const {
    auto value = GetPropertyMap().TryGetProperty<bool>(kHasTitleBarPropertyName);
    return value ? *value : true;
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


bool Window::IsTopmost() const {

    auto is_set = GetPropertyMap().TryGetProperty<bool>(kIsTopmostPropertyName);
    if (is_set) {
        return *is_set;
    }
    return false;
}


void Window::SetIsTopmost(bool is_topmost) {
    SetStyleProperty(kIsTopmostPropertyName, WS_EX_TOPMOST, is_topmost, true);
}


void Window::SetStyleProperty(
    const std::wstring& property_name,
    DWORD style_value,
    bool is_set,
    bool is_extra_style) {

    GetPropertyMap().SetProperty(property_name, is_set);

    if (! !Handle()) {
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
        GetWindowText(handle_, buffer.data(), static_cast<int>(buffer.size()));
        return buffer.data();
    }
}


void Window::SetTitle(const std::wstring& title) {

    GetPropertyMap().SetProperty(kTitlePropertyName, title);

    if (! !Handle()) {
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

    if (!!Handle()) {
        RECT client_rect{};
        ::GetClientRect(handle_, &client_rect);
        root_control_->SetRect(Rect::FromRECT(client_rect));
    }

    OnRootControlChanged(previous_root_control);
}


const std::shared_ptr<Caret>& Window::Caret() {

    if (caret_ == nullptr) {
        caret_ = std::make_shared<zaf::Caret>();
        caret_->SetWindow(shared_from_this());
    }
    return caret_;
}


Observable<WindowCloseInfo> Window::CloseEvent() {
    return GetEventObservable<WindowCloseInfo>(GetPropertyMap(), kCloseEventPropertyName);
}


Observable<WindowDestroyInfo> Window::DestroyEvent() {
    return GetEventObservable<WindowDestroyInfo>(GetPropertyMap(), kDestroyEventPropertyName);
}


Observable<WindowReceiveMessageInfo> Window::ReceiveMessageEvent() {
    return GetEventObservable<WindowReceiveMessageInfo>(
        GetPropertyMap(), 
        kReceiveMessageEventPropertyName);
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


float Window::GetDPI() const {

    if (Handle()) {
        return static_cast<float>(GetDpiForWindow(Handle()));
    }
    else {
        return static_cast<float>(GetDpiForSystem());
    }
}


void Window::Show() {

    CheckCreateWindowHandle();

    auto activate_option = ActivateOption();
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


bool Window::IsVisible() const {
    return !!IsWindowVisible(handle_);
}


void Window::Close() {
    SendMessage(Handle(), WM_CLOSE, 0, 0);
}


void Window::Destroy() {
    if (handle_) {
        DestroyWindow(handle_);
    }
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
        NeedRepaintRect(highlight_control_->AbsoluteRect());
    }

    highlight_control_ = highlight_control;

    //Repaint the rect of new highlight control.
    NeedRepaintRect(highlight_control_->AbsoluteRect());
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