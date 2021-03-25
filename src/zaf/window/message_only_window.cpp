#include <zaf/window/message_only_window.h>
#include <zaf/base/error/system_error.h>
#include <zaf/window/message/creation.h>

namespace zaf {
namespace {

constexpr wchar_t* const MessageOnlyWindowClassName = L"ZafMessageOnlyWindowClass";

}

MessageOnlyWindow::MessageOnlyWindow() {

	RegisterWindowClass();

	handle_ = CreateWindowEx(
		0,
		MessageOnlyWindowClassName,
		L"",
		0,
		0,
		0,
		0,
		0,
		HWND_MESSAGE,
		nullptr,
		nullptr,
		nullptr);

	if (!handle_) {
		ZAF_THROW_SYSTEM_ERROR(GetLastError());
	}

	SetLastError(ERROR_SUCCESS);

	SetWindowLongPtr(
		handle_,
		GWLP_USERDATA,
		reinterpret_cast<ULONG_PTR>(this));

	ZAF_THROW_IF_SYSTEM_ERROR(GetLastError());
}


MessageOnlyWindow::~MessageOnlyWindow() {

    DestroyWindow(handle_);
}


void MessageOnlyWindow::RegisterWindowClass() {

	static ATOM atom = []() {
	
		WNDCLASSEX default_class {};
		default_class.cbSize = sizeof(default_class);
		default_class.lpszClassName = MessageOnlyWindowClassName;
		default_class.lpfnWndProc = WindowProcedure;
		default_class.cbWndExtra = sizeof(LONG_PTR);

		ATOM atom = RegisterClassEx(&default_class);
		if (atom == 0) {
			ZAF_THROW_SYSTEM_ERROR(GetLastError());
		}

		return atom;
	}();
}


LRESULT CALLBACK MessageOnlyWindow::WindowProcedure(
	HWND hwnd, 
	UINT id, 
	WPARAM wparam, 
	LPARAM lparam) {

	LONG_PTR user_data = GetWindowLongPtr(hwnd, GWLP_USERDATA);
	auto window = reinterpret_cast<MessageOnlyWindow*>(user_data);
	if (window) {
		window->OnReceiveMessage(id, wparam, lparam);
	}

	return CallWindowProc(DefWindowProc, hwnd, id, wparam, lparam);
}


void MessageOnlyWindow::OnReceiveMessage(UINT id, WPARAM wparam, LPARAM lparam) {

	auto message = CreateMessage(handle_, id, wparam, lparam);
	subject_.GetObserver().OnNext(*message);
}

}