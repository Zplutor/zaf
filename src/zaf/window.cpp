#include <zaf/window.h>
#include <Windowsx.h>
#include <zaf/application.h>
#include <zaf/canvas.h>
#include <zaf/caret.h>
#include <zaf/renderer.h>
#include <zaf/internal/conversion.h>
#include <zaf/internal/log.h>

namespace zaf {

static const wchar_t* const kDefaultWindowClassName = L"ZafDefaultWindowClass";

bool Window::RegisterDefaultClass() {

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
	return atom != 0;
}


LRESULT CALLBACK Window::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

	LONG_PTR user_data = GetWindowLongPtr(hwnd, GWLP_USERDATA);
	Window* window = reinterpret_cast<Window*>(user_data);

	switch (message) {
	case WM_PAINT:
		window->Repaint();
		return 0;

	case WM_SIZE:
		window->Resize(LOWORD(lParam), HIWORD(lParam));
		return 0;

	case WM_SETCURSOR: {
		bool is_changed = window->ChangeMouseCursor(wParam, lParam);		
		if (is_changed) {
			return TRUE;
		}
		break;
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
		window->ReceiveMouseMessage(message, wParam, lParam);
		return 0;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
		window->ReceiveKeyMessage(message, wParam, lParam);
		return 0;

	case WM_KILLFOCUS:
		window->LostFocus();
		return 0;

	default:
		break;
	}

	return CallWindowProc(DefWindowProc, hwnd, message, wParam, lParam);
}


Window::Window() :
	handle_(NULL),
	state_(std::make_shared<internal::WindowNotCreatedState>()),
	is_tracking_mouse_(false),
	is_capturing_mouse_(false),
	root_control_(std::make_shared<Control>()),
	close_event_(),
	OnClose(close_event_) {


}


Window::~Window() {

}


void Window::Show() {

	CheckCreate();
	ShowWindow(handle_, SW_SHOW);
}


void Window::CheckCreate() {

	if (handle_ != NULL) {
		return;
	}

	Rect rect = GetRect();
	std::wstring title = GetTitle();

	handle_ = CreateWindowEx(
		0,
		kDefaultWindowClassName,
		title.c_str(),
		WS_OVERLAPPEDWINDOW,
		static_cast<int>(rect.position.x), 
		static_cast<int>(rect.position.y), 
		static_cast<int>(rect.size.width), 
		static_cast<int>(rect.size.height),
		NULL,
		NULL,
		NULL,
		nullptr
	);

	SetWindowLongPtr(handle_, GWLP_USERDATA, reinterpret_cast<ULONG_PTR>(this));
	state_ = std::make_shared<internal::WindowCreatedState>(handle_);

	RECT rc;
	GetClientRect(handle_, &rc);

	D2D1_SIZE_U client_size = D2D1::SizeU(
		rc.right - rc.left,
		rc.bottom - rc.top
	);

	ID2D1HwndRenderTarget* renderer_handle = nullptr;
	D2D1_RENDER_TARGET_PROPERTIES properties = D2D1::RenderTargetProperties();
	properties.usage |= D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE;
	properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
	Application::GetInstance().GetD2DFactory()->CreateHwndRenderTarget(
		properties,
		D2D1::HwndRenderTargetProperties(handle_, client_size),
		&renderer_handle
	);

	root_control_->CheckInitialized();
	root_control_->SetWindow(this->shared_from_this());
	renderer_ = std::make_shared<Renderer>(renderer_handle);

	Application::GetInstance().RegisterWindow(shared_from_this());
}


void Window::Hide() {

}


void Window::Close() {

}


const Point Window::GetMousePosition() const {

	POINT cursor_point = { 0 };
	GetCursorPos(&cursor_point);
	ScreenToClient(handle_, &cursor_point);

	return Point(static_cast<float>(cursor_point.x), static_cast<float>(cursor_point.y));
}


void Window::NeedRepaintRect(const Rect& rect) {

	//LOG() << "NeedRepaint";

	RECT win32_rect = ToWin32Rect(rect);
	InvalidateRect(handle_, &win32_rect, FALSE);
}


void Window::Repaint() {

	Rect dirty_rect;

	RECT win32_rect;
	if (GetUpdateRect(handle_, &win32_rect, TRUE)) {
		dirty_rect = FromWin32Rect(win32_rect);
	}
	else {
		dirty_rect = root_control_->GetRect();
	}

	renderer_->BeginRender();

	Canvas canvas(renderer_, root_control_->GetRect(), dirty_rect);
	root_control_->Repaint(canvas, dirty_rect);
	
	if (caret_ != nullptr) {
		const Rect& caret_rect = caret_->GetRect();
		if (caret_rect.HasIntersection(dirty_rect)) {
			caret_->Paint(canvas);
		}
	}

	renderer_->EndRender();

	ValidateRect(handle_, nullptr);
}


void Window::Resize(UINT width, UINT height) {

	renderer_->GetHandle()->Resize(D2D1::SizeU(width, height));
	root_control_->SetRect(Rect(0, 0, static_cast<float>(width), static_cast<float>(height)));
}


void Window::LostFocus() {

	if (hovered_control_ != nullptr && hovered_control_->IsCapturingMouse()) {
		SetCaptureMouseControl(hovered_control_, true);
	}
}


bool Window::ChangeMouseCursor(WPARAM wParam, LPARAM lParam) {

	bool is_changed = false;

	if (hovered_control_ != nullptr) {
		hovered_control_->ChangeMouseCursor(wParam, lParam, is_changed);
	}

	return is_changed;
}


void Window::ReceiveMouseMessage(UINT message, WPARAM wParam, LPARAM lParam) {

	Point point(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)));

	switch (message) {
	case WM_MOUSEMOVE: {
		if (! is_tracking_mouse_) {
			TRACKMOUSEEVENT track_mouse_event_param = { 0 };
			track_mouse_event_param.cbSize = sizeof(track_mouse_event_param);
			track_mouse_event_param.dwFlags = TME_LEAVE;
			track_mouse_event_param.hwndTrack = handle_;
			if (TrackMouseEvent(&track_mouse_event_param)) {
				is_tracking_mouse_ = true;
			}
		}
		if (! is_capturing_mouse_) {
			root_control_->RouteHoverMessage(point);
		}
		break;
	}
	case WM_MOUSELEAVE:
		is_tracking_mouse_ = false;
		if (! is_capturing_mouse_) {
			SetHoveredControl(nullptr);
		}
		break;

	case WM_MOUSEWHEEL:
	case WM_MOUSEHWHEEL: {
		//The cursor position in these two messages is in screen coordinate.
		//Translate it to window coordinate.
		POINT cursor_point = ToWin32Point(point);
		ScreenToClient(handle_, &cursor_point);
		point = FromWin32Point(cursor_point);
		break;
	}
	}

	if (is_capturing_mouse_) {

		//Send message to the hovering control directly.
		Rect control_rect = hovered_control_->GetAbsoluteRect();
		Point related_point(point.x - control_rect.position.x, point.y - control_rect.position.y);
		hovered_control_->InterpretMessage(related_point, message, wParam, lParam);
	}
	else {
		
		root_control_->RouteMessage(point, message, wParam, lParam);
	}
}


void Window::ReceiveKeyMessage(UINT message, WPARAM wParam, LPARAM lParam) {

	if (focused_control_ == nullptr) {
		return;
	}

	switch (message) {
	case WM_KEYDOWN:
		focused_control_->KeyDown(wParam, lParam);
		break;

	case WM_KEYUP:
		focused_control_->KeyUp(wParam, lParam);
		break;

	case WM_CHAR:
		focused_control_->CharInput(wParam, lParam);
		break;
	}
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


const std::shared_ptr<Caret>& Window::GetCaret() {

	if (caret_ == nullptr) {
		caret_ = std::make_shared<Caret>();
		caret_->SetWindow(shared_from_this());
	}
	return caret_;
}

}