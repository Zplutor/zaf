#include <zaf/window/window.h>
#include <Windowsx.h>
#include <zaf/application.h>
#include <zaf/base/error.h>
#include <zaf/base/event_utility.h>
#include <zaf/creation.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/clear_edge.h>
#include <zaf/graphic/resource_factory.h>
#include <zaf/internal/tab_stop_utility.h>
#include <zaf/internal/theme.h>
#include <zaf/serialization/properties.h>
#include <zaf/window/caret.h>
#include <zaf/window/message/creation.h>
#include <zaf/window/message/hit_test_message.h>
#include <zaf/window/message/keyboard_message.h>
#include <zaf/window/message/message.h>
#include <zaf/window/message/mouse_message.h>

namespace zaf {

static const wchar_t* const kDefaultWindowClassName = L"ZafDefaultWindowClass";

static const wchar_t* const kActivateOptionPropertyName = L"ActivateOption";
static const wchar_t* const kBorderStylePropertyName = L"BorderStyle";
static const wchar_t* const kCloseEventPropertyName = L"CloseEvent";
static const wchar_t* const kCloseHandlerPropertyName = L"CloseHandler";
static const wchar_t* const kHasMaximizeButtonPropertyName = L"HasMaximizeButton";
static const wchar_t* const kHasMinimizeButtonPropertyName = L"HasMinimizeButton";
static const wchar_t* const kHasSystemMenuPropertyName = L"HasSystemMenu";
static const wchar_t* const kInitialRectStylePropertyName = L"InitialRectStyle";
static const wchar_t* const kIsSizablePropertyName = L"IsSizable";
static const wchar_t* const kOwnerPropertyName = L"Owner";
static const wchar_t* const kTitlePropertyName = L"Title";

void Window::RegisterDefaultClass(std::error_code& error_code) {

	WNDCLASSEX default_class = { 0 };
	default_class.cbSize = sizeof(default_class);
	default_class.style = CS_HREDRAW | CS_VREDRAW;
	default_class.lpfnWndProc = WindowProcedure;
	default_class.cbClsExtra = 0;
	default_class.cbWndExtra = sizeof(LONG_PTR);
	default_class.hInstance = NULL;
	default_class.hIcon = NULL;
	default_class.hCursor = LoadCursor(NULL, IDI_APPLICATION);
	default_class.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	default_class.lpszMenuName = nullptr;
	default_class.lpszClassName = kDefaultWindowClassName;
	default_class.hIconSm = NULL;

	ATOM atom = RegisterClassEx(&default_class);
    if (atom != 0) {
        error_code.clear();
    }
    else {
        error_code = MakeSystemErrorCode(GetLastError());
    }
}


LRESULT CALLBACK Window::WindowProcedure(HWND hwnd, UINT message_id, WPARAM wparam, LPARAM lparam) {

    auto window = GetWindowFromHandle(hwnd);
    if (window != nullptr) {

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
    InitializeRootControl(Create<Control>());
}


void Window::CreateWindowHandle() {

    auto owner = GetOwner();
    auto initial_rect = GetInitialRect();

    DWORD style = 0;
    DWORD extract_style = 0;
    GetStyles(style, extract_style);

    handle_ = CreateWindowEx(
        extract_style,
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
        nullptr);

    SetWindowLongPtr(handle_, GWLP_USERDATA, reinterpret_cast<ULONG_PTR>(this));
    rect_ = initial_rect;

    RECT client_rect = { 0 };
    ::GetClientRect(handle_, &client_rect);
    root_control_->SetRect(Rect::FromRECT(client_rect));

    CreateRenderer();
    Application::GetInstance().RegisterWindow(shared_from_this());

    WindowCreate();
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

    renderer_ = GetResourceFactory()->CreateWindowRenderer(handle_);
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


void Window::GetStyles(DWORD& style, DWORD& extract_style) const {

    auto border_style = GetBorderStyle();
    switch (border_style) {

        case BorderStyle::None:
            style |= WS_POPUP;
            break;

        case BorderStyle::Dialog:
            style |= WS_POPUP | WS_CAPTION;
            extract_style |= WS_EX_DLGMODALFRAME;
            break;

        case BorderStyle::ToolWindow:
            style |= WS_POPUP | WS_CAPTION;
            extract_style |= WS_EX_TOOLWINDOW;
            break;
            
        case BorderStyle::Normal:
            style |= WS_OVERLAPPED | WS_CAPTION;
            break;
    }

    if (border_style != BorderStyle::None) {

        if (IsSizable()) {
            style |= WS_SIZEBOX;
        }

        if (HasSystemMenu()) {
            style |= WS_SYSMENU;
        }

        if (HasMaximizeButton()) {
            style |= WS_MAXIMIZEBOX;
        }

        if (HasMinimizeButton()) {
            style |= WS_MINIMIZEBOX;
        }
    }

    auto activate_option = GetActivateOption();
    if ((activate_option & ActivateOption::NoActivate) == ActivateOption::NoActivate) {
        extract_style |= WS_EX_NOACTIVATE;
    }
}


bool Window::PreprocessMessage(const KeyMessage& message) {

    if ((message.id != WM_KEYDOWN) || (message.GetVirtualKey() != VK_TAB)) {
        return false;
    }

    if ((focused_control_ != nullptr) && focused_control_->AcceptKeyMessage(message)) {
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


bool Window::ReceiveMessage(const Message& message, LRESULT& result) {

    switch (message.id) {
    case WM_ERASEBKGND:
        result = TRUE;
        return true;

    case WM_PAINT:
        Repaint();
        return true;

    case WM_GETMINMAXINFO: {
        auto min_max_info = reinterpret_cast<MINMAXINFO*>(message.lparam);
        min_max_info->ptMinTrackSize.x = static_cast<LONG>(GetMinimumWidth());
        min_max_info->ptMinTrackSize.y = static_cast<LONG>(GetMinimumHeight());
        min_max_info->ptMaxTrackSize.x = static_cast<LONG>(GetMaximumWidth());
        min_max_info->ptMaxTrackSize.y = static_cast<LONG>(GetMaximumHeight());
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

            CapturingMouseControlChange(previous_control);
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
            return focused_control_->KeyDown(dynamic_cast<const KeyMessage&>(message));
        }
        return false;

    case WM_KEYUP:
        if (focused_control_ != nullptr) {
            return focused_control_->KeyUp(dynamic_cast<const KeyMessage&>(message));
        }
        return false;

    case WM_CHAR:
        if (focused_control_ != nullptr) {
            return focused_control_->CharInput(dynamic_cast<const CharMessage&>(message));
        }
        return false;

    case WM_CLOSE:
        return ! ReceiveCloseMessage();

    case WM_DESTROY:
        ReceiveDestroyMessage();
        return true;

    case WM_NCDESTROY:
        Application::GetInstance().UnregisterWindow(shared_from_this());
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
        dirty_rect = root_control_->GetRect();
    }

    //The update rect must be validated before painting.
    //Because some controls may call NeedRepaint while it is painting,
    //and this may fails if there is a invalidated update rect.
    ValidateRect(handle_, nullptr);

    renderer_.BeginDraw();

    Canvas canvas(renderer_, root_control_->GetRect(), dirty_rect);
    root_control_->Repaint(canvas, dirty_rect);

    if (caret_ != nullptr) {
        const Rect& caret_rect = caret_->GetRect();
        if (caret_rect.HasIntersection(dirty_rect)) {
            caret_->Repaint(canvas);
        }
    }

    std::error_code error_code;
    renderer_.EndDraw(error_code);

    if (IsComErrorCode(error_code, D2DERR_RECREATE_TARGET)) {
        RecreateRenderer();
    }
}


void Window::NeedRepaintRect(const Rect& rect) {

    if (handle_ != nullptr) {
        RECT win32_rect = MakeClearEdgeForFill(rect, ClearEdgeOption::Clear).ToRECT();
        InvalidateRect(handle_, &win32_rect, FALSE);
    }
}


void Window::Resize(UINT width, UINT height) {

    Size size(static_cast<float>(width), static_cast<float>(height));

    renderer_.Resize(size);
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

        TrackMouseLeave(message);

        if (is_capturing_mouse) {
            capturing_mouse_control_->RouteHoverMessage(get_mouse_position_to_capturing_control());
        }
        else {
            root_control_->RouteHoverMessage(message.GetMousePosition());
        }
    }
    else if (message.id == WM_MOUSELEAVE || message.id == WM_NCMOUSELEAVE) {
        MouseLeave(message);
    }

    if (is_capturing_mouse) {
        return capturing_mouse_control_->RouteMessage(get_mouse_position_to_capturing_control(), message);
    }
    else {
        return root_control_->RouteMessage(message.GetMousePosition(), message);
    }
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


void Window::MouseLeave(const MouseMessage& message) {

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
        SetHoveredControl(nullptr);
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
    if (can_close) {

        auto close_event = TryGetEventFromPropertyMap<CloseEvent>(GetPropertyMap(), kCloseEventPropertyName);
        if (close_event != nullptr) {
            close_event->Trigger(shared_from_this());
        }
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

    HWND old_handle = handle_;

    handle_ = nullptr;
    renderer_.Reset();

    WindowDestroy(old_handle);
}


void Window::SetHoveredControl(const std::shared_ptr<Control>& hovered_control) {

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
            MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));

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

    CapturingMouseControlChange(previous_control);
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

    FocusedControlChange(previous_focused_control);
}


std::shared_ptr<Window> Window::GetOwner() const {

    auto owner = GetPropertyMap().TryGetProperty<std::shared_ptr<Window>>(kOwnerPropertyName);
    if (owner != nullptr) {
        return *owner;
    }
    else {
        return nullptr;
    }
}


void Window::SetOwner(const std::shared_ptr<Window>& owner) {

    if (IsClosed()) {
        GetPropertyMap().SetProperty(kOwnerPropertyName, owner);
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
            SWP_NOZORDER);
    }
}


float Window::GetMinimumWidth() const {

    auto width = GetPropertyMap().TryGetProperty<float>(property::MinimumWidth);
    if (width != nullptr) {
        return *width;
    }
    return static_cast<float>(GetSystemMetrics(SM_CXMINTRACK));
}


void Window::SetMinimumWidth(float min_width) {

    GetPropertyMap().SetProperty(property::MinimumWidth, min_width);

    if (GetMaximumWidth() < min_width) {
        SetMaximumWidth(min_width);
    }

    if (GetWidth() < min_width) {
        SetWidth(min_width);
    }
}


float Window::GetMaximumWidth() const {

    auto width = GetPropertyMap().TryGetProperty<float>(property::MaximumWidth);
    if (width != nullptr) {
        return *width;
    }
    return static_cast<float>(GetSystemMetrics(SM_CXMAXTRACK));
}


void Window::SetMaximumWidth(float max_width) {

    GetPropertyMap().SetProperty(property::MaximumWidth, max_width);

    if (GetMinimumWidth() > max_width) {
        SetMinimumWidth(max_width);
    }

    if (GetWidth() > max_width) {
        SetWidth(max_width);
    }
}


float Window::GetMinimumHeight() const {

    auto height = GetPropertyMap().TryGetProperty<float>(property::MinimumHeight);
    if (height != nullptr) {
        return *height;
    }
    return static_cast<float>(GetSystemMetrics(SM_CYMINTRACK));
}


void Window::SetMinimumHeight(float min_height) {

    GetPropertyMap().SetProperty(property::MinimumHeight, min_height);

    if (GetMaximumHeight() < min_height) {
        SetMaximumHeight(min_height);
    }

    if (GetHeight() < min_height) {
        SetHeight(min_height);
    }
}


float Window::GetMaximumHeight() const {

    auto height = GetPropertyMap().TryGetProperty<float>(property::MaximumHeight);
    if (height != nullptr) {
        return *height;
    }
    return static_cast<float>(GetSystemMetrics(SM_CYMAXTRACK));
}


void Window::SetMaximumHeight(float max_height) {

    GetPropertyMap().SetProperty(property::MaximumHeight, max_height);

    if (GetMinimumHeight() > max_height) {
        SetMaximumHeight(max_height);
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


Window::BorderStyle Window::GetBorderStyle() const {

    auto border_style = GetPropertyMap().TryGetProperty<BorderStyle>(kBorderStylePropertyName);
    if (border_style != nullptr) {
        return *border_style;
    }
    else {
        return BorderStyle::Normal;
    }
}


void Window::SetBorderStyle(BorderStyle border_style) {

    if (IsClosed()) {
        GetPropertyMap().SetProperty(kBorderStylePropertyName, border_style);
    }
}


bool Window::IsSizable() const {
    return GetStyleProperty(kIsSizablePropertyName, WS_SIZEBOX);
}


void Window::SetIsSizable(bool is_sizable) {
    SetStyleProperty(kIsSizablePropertyName, WS_SIZEBOX, is_sizable);
}


bool Window::HasSystemMenu() const {
    return GetStyleProperty(kHasSystemMenuPropertyName, WS_SYSMENU);
}


void Window::SetHasSystemMenu(bool has_system_menu) {
    SetStyleProperty(kHasSystemMenuPropertyName, WS_SYSMENU, has_system_menu);
}


bool Window::HasMinimizeButton() const {
    return GetStyleProperty(kHasMinimizeButtonPropertyName, WS_MINIMIZEBOX);
}

void Window::SetHasMinimizeButton(bool has_minimize_button) {
    SetStyleProperty(kHasMinimizeButtonPropertyName, WS_MINIMIZEBOX, has_minimize_button);
}


bool Window::HasMaximizeButton() const {
    return GetStyleProperty(kHasMaximizeButtonPropertyName, WS_MAXIMIZEBOX);
}

void Window::SetHasMaximizeButton(bool has_maximize_button) {
    SetStyleProperty(kHasMaximizeButtonPropertyName, WS_MAXIMIZEBOX, has_maximize_button);
}


bool Window::GetStyleProperty(const std::wstring& property_name, DWORD style_value) const {

    if (GetBorderStyle() == BorderStyle::None) {
        return false;
    }

    if (IsClosed()) {

        auto property_value = GetPropertyMap().TryGetProperty<bool>(property_name);
        if (property_value != nullptr) {
            return *property_value;
        }
        else {
            return true;
        }
    }
    else {
        return (GetWindowLong(GetHandle(), GWL_STYLE) & style_value) != 0;
    }
}


void Window::SetStyleProperty(const std::wstring& property_name, DWORD style_value, bool is_set) {

    if (GetBorderStyle() == BorderStyle::None) {
        return;
    }

    GetPropertyMap().SetProperty(property_name, is_set);

    if (! IsClosed()) {

        DWORD style = GetWindowLong(GetHandle(), GWL_STYLE);
        if (is_set) {
            style |= style_value;
        }
        else {
            style &= ~style_value;
        }
        SetWindowLong(GetHandle(), GWL_STYLE, style);
    }
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

    RootControlChange(previous_root_control);
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


Window::CloseEvent::Proxy Window::GetCloseEvent() {

    auto& event = GetPropertyMap().GetProperty<std::shared_ptr<CloseEvent>>(
        kCloseEventPropertyName,
        []() { return std::make_shared<CloseEvent>(); }
    );
    return CloseEvent::Proxy(*event);
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

    WindowShow();
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


const std::shared_ptr<Window> GetWindowFromHandle(HWND handle) {

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