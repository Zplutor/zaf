#include <zaf/window/window.h>
#include <Windowsx.h>
#include <zaf/application.h>
#include <zaf/base/error.h>
#include <zaf/base/event_utility.h>
#include <zaf/base/log.h>
#include <zaf/creation.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/clear_edge.h>
#include <zaf/graphic/renderer.h>
#include <zaf/graphic/renderer_factory.h>
#include <zaf/internal/theme.h>
#include <zaf/window/caret.h>
#include <zaf/window/message/creation.h>
#include <zaf/window/message/key_message.h>
#include <zaf/window/message/message.h>
#include <zaf/window/message/mouse_message.h>

namespace zaf {

static const wchar_t* const kDefaultWindowClassName = L"ZafDefaultWindowClass";

static const wchar_t* const kCloseEventPropertyName = L"CloseEvent";
static const wchar_t* const kCloseHandlerPropertyName = L"CloseHandler";
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


LRESULT CALLBACK Window::WindowProcedure(HWND hwnd, UINT message_id, WPARAM wParam, LPARAM lParam) {

	LONG_PTR user_data = GetWindowLongPtr(hwnd, GWLP_USERDATA);
	Window* window = reinterpret_cast<Window*>(user_data);

    if (window != nullptr) {

        //Keep the windows's life during message processing.
        auto shared_ptr = window->shared_from_this();

        auto message = CreateMessage(hwnd, message_id, wParam, lParam);
        if (message != nullptr) {

            LRESULT result = 0;
            if (window->ReceiveMessage(*message, result)) {
                return result;    
            }
        }
    }

	return CallWindowProc(DefWindowProc, hwnd, message_id, wParam, lParam);
}


Window::Window() :
	handle_(nullptr),
	is_tracking_mouse_(false),
	is_capturing_mouse_(false) {

}


Window::~Window() {

    if (handle_ != nullptr) {
        DestroyWindow(handle_);
    }
}


void Window::Initialize() {

    root_control_ = Create<Control>();
    root_control_->SetWindow(shared_from_this());
    root_control_->SetBackgroundColorPicker([](const Control&) {
        return internal::WindowBackgroundColor;
    });

    int screen_width = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);
    int window_width = screen_width / 2;
    int window_height = screen_height / 2;
    rect_.position.x = static_cast<float>((screen_width - window_width) / 2);
    rect_.position.y = static_cast<float>((screen_height - window_height) / 2);
    rect_.size.width = static_cast<float>(window_width);
    rect_.size.height = static_cast<float>(window_height);
}


void Window::CreateWindowHandle() {

    auto owner = GetOwner();
    auto rect = GetRect();

    handle_ = CreateWindowEx(
        0,
        kDefaultWindowClassName,
        GetTitle().c_str(),
        WS_OVERLAPPEDWINDOW,
        static_cast<int>(rect.position.x),
        static_cast<int>(rect.position.y),
        static_cast<int>(rect.size.width),
        static_cast<int>(rect.size.height),
        owner == nullptr ? nullptr : owner->GetHandle(),
        nullptr,
        nullptr,
        nullptr
    );

    SetWindowLongPtr(handle_, GWLP_USERDATA, reinterpret_cast<ULONG_PTR>(this));

    CreateRenderer();
    Application::GetInstance().RegisterWindow(shared_from_this());

    WindowCreate();
}


void Window::CreateRenderer() {

    renderer_ = Application::GetInstance().GetRendererFactory()->CreateRenderer(handle_);
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


bool Window::ReceiveMessage(const Message& message, LRESULT& result) {

    switch (message.id) {
    case WM_PAINT:
        Repaint();
        return true;

    case WM_SIZE:
        Resize(LOWORD(message.lParam), HIWORD(message.lParam));
        return true;

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

    case WM_MOUSEMOVE:
    case WM_MOUSELEAVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MOUSEWHEEL:
    case WM_MOUSEHWHEEL:
        ReceiveMouseMessage(dynamic_cast<const MouseMessage&>(message));
        return true;

    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_CHAR:
        ReceiveKeyMessage(dynamic_cast<const KeyMessage&>(message));
        return true;

    case WM_KILLFOCUS:
        LostFocus();
        return true;

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

    renderer_->BeginDraw();

    Canvas canvas(renderer_);
    canvas.SetRects(root_control_->GetRect(), dirty_rect);
    root_control_->Repaint(canvas, dirty_rect);

    if (caret_ != nullptr) {
        const Rect& caret_rect = caret_->GetRect();
        if (caret_rect.HasIntersection(dirty_rect)) {
            canvas.SetRects(root_control_->GetRect(), dirty_rect);
            caret_->Repaint(canvas);
        }
    }

    std::error_code error_code;
    renderer_->EndDraw(error_code);

    if (IsComErrorCode(error_code, D2DERR_RECREATE_TARGET)) {
        RecreateRenderer();
    }
}


void Window::NeedRepaintRect(const Rect& rect) {

    RECT win32_rect = MakeClearEdgeRectForFill(rect, ClearEdgeOption::Clear).ToRECT();
    InvalidateRect(handle_, &win32_rect, FALSE);
}


void Window::Resize(UINT width, UINT height) {

    Size size(static_cast<float>(width), static_cast<float>(height));

    renderer_->Resize(size);
    root_control_->SetRect(Rect(Point(), size));
}


void Window::ReceiveMouseMessage(const MouseMessage& message) {

    if (message.id == WM_MOUSEMOVE) {

        if (!is_tracking_mouse_) {

            TRACKMOUSEEVENT track_mouse_event_param = { 0 };
            track_mouse_event_param.cbSize = sizeof(track_mouse_event_param);
            track_mouse_event_param.dwFlags = TME_LEAVE;
            track_mouse_event_param.hwndTrack = handle_;
            if (TrackMouseEvent(&track_mouse_event_param)) {
                is_tracking_mouse_ = true;
            }
        }

        if (!is_capturing_mouse_) {
            root_control_->RouteHoverMessage(message.position);
        }
    }
    else if (message.id == WM_MOUSELEAVE) {

        is_tracking_mouse_ = false;
        if (!is_capturing_mouse_) {
            SetHoveredControl(nullptr);
        }    
    }

    if (is_capturing_mouse_) {

        //Send message to the hovering control directly.
        Rect control_rect = hovered_control_->GetAbsoluteRect();
        Point related_position(
            message.position.x - control_rect.position.x,
            message.position.y - control_rect.position.y
        );

        hovered_control_->InterpretMessage(related_position, message);
    }
    else {

        root_control_->RouteMessage(message.position, message);
    }
}


void Window::ReceiveKeyMessage(const KeyMessage& message) {

    if (focused_control_ == nullptr) {
        return;
    }

    switch (message.id) {
    case WM_KEYDOWN:
        focused_control_->KeyDown(message);
        break;

    case WM_KEYUP:
        focused_control_->KeyUp(message);
        break;

    case WM_CHAR:
        focused_control_->CharInput(message);
        break;
    }
}


bool Window::ChangeMouseCursor(const Message& message) {

    bool is_changed = false;

    if (hovered_control_ != nullptr) {
        hovered_control_->ChangeMouseCursor(message, is_changed);
    }

    return is_changed;
}


void Window::LostFocus() {

    if (hovered_control_ != nullptr && hovered_control_->IsCapturingMouse()) {
        SetCaptureMouseControl(hovered_control_, true);
    }
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

    HWND old_handle = handle_;

    handle_ = nullptr;
    renderer_ = nullptr;

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
		hovered_control_->IsHoveredChanged(true);
	}
}


void Window::SetCaptureMouseControl(const std::shared_ptr<Control>& control, bool is_releasing) {

	if ( (hovered_control_ == nullptr) || (control == nullptr) ) {
		return;
	}

	if (hovered_control_ != control) {
		return;
	}

	if (! is_releasing) {
		SetCapture(handle_);
	}
	else {
		ReleaseCapture();
	}

	is_capturing_mouse_ = ! is_releasing;
	hovered_control_->IsCapturingMouseChanged(! is_releasing);
}


void Window::SetFocusedControl(const std::shared_ptr<Control>& focused_control) {

	if (focused_control_ == focused_control) {
		return;
	}

	if (focused_control != nullptr) {

		if (! focused_control->IsEnabled()) {
			return;
		}

		//The focused control must be contained in this window
		if (focused_control->GetWindow().get() != this) {
			return;
		}
	}

	if (focused_control_ != nullptr) {
		focused_control_->IsFocusedChanged(false);
	}

	focused_control_ = focused_control;

	if (focused_control_ != nullptr) {
		focused_control_->IsFocusedChanged(true);
	}
}


const std::shared_ptr<Window> Window::GetOwner() const {

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


const Rect Window::GetRect() const {

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

    SetWindowPos(
        handle_,
        nullptr,
        static_cast<int>(rect_.position.x),
        static_cast<int>(rect_.position.y),
        static_cast<int>(rect_.size.width),
        static_cast<int>(rect_.size.height),
        SWP_NOZORDER
    );
}


const Rect Window::GetClientRect() const {

    RECT rect = { 0 };
    ::GetClientRect(handle_, &rect);
    return Rect::FromRECT(rect);
}


const std::wstring Window::GetTitle() const {

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
    ShowWindow(handle_, SW_SHOW);
}


void Window::Hide() {

    ShowWindow(handle_, SW_HIDE);
}


void Window::Close() {

    SendMessage(GetHandle(), WM_CLOSE, 0, 0);
}


}