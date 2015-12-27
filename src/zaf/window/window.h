#pragma once

#include <Windows.h>
#include <zaf/control.h>
#include <zaf/base/event.h>
#include <zaf/base/rect.h>
#include <zaf/window/window_state.h>

namespace zaf {

class Caret;
class Renderer;

class Window : public std::enable_shared_from_this<Window> {
public:
	typedef Event<const std::shared_ptr<Window>&> CloseEvent;

public:
	Window();
	~Window();

	/**
	 Get position of the mouse cursor in current window's coordinate system.
	 */
	const Point GetMousePosition() const;

	const std::shared_ptr<Control>& GetRootControl() const {
		return root_control_;
	}

	/**
	 Get the control which has input focus in this window.
 	 */
	const std::shared_ptr<Control>& GetFocusedControl() const {
		return focused_control_;
	}

	const std::shared_ptr<Caret>& GetCaret();

	const Rect GetRect() const {
		return state_->GetRect();
	}

	void SetRect(const Rect& rect) {
		state_->SetRect(rect);
	}

	const std::wstring GetTitle() const {
		return state_->GetTitle();
	}

	void SetTitle(const std::wstring& title) {
		state_->SetTitle(title);
	}

	void Show();
	void Hide();
	void Close();

	HWND GetHandle() const {
		return handle_;
	}

	const std::shared_ptr<Renderer>& GetRenderer() const {
		return renderer_;
	}

public:
	CloseEvent::Proxy OnClose;

private:
	friend class Application;
	friend class Caret;
	friend class Control;

	static bool RegisterDefaultClass();

	void NeedRepaintRect(const Rect& rect);
	void SetHoveredControl(const std::shared_ptr<Control>& hovered_control);
	void SetCaptureMouseControl(const std::shared_ptr<Control>& hovered_control, bool is_releasing);
	void SetFocusedControl(const std::shared_ptr<Control>& focused_control);

private:
	static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	void CheckCreate();
	void Repaint();
	void Resize(UINT width, UINT height);
	void LostFocus();
	bool ChangeMouseCursor(WPARAM wParam, LPARAM lParam);
	void ReceiveMouseMessage(UINT message, WPARAM wParam, LPARAM lParam);
	void ReceiveKeyMessage(UINT message, WPARAM wParam, LPARAM lParam);

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

private:
	HWND handle_;
	std::shared_ptr<internal::WindowState> state_;
	std::shared_ptr<Renderer> renderer_;
	bool is_tracking_mouse_;
	bool is_capturing_mouse_;

	std::shared_ptr<Control> root_control_;
	std::shared_ptr<Control> hovered_control_;
	std::shared_ptr<Control> focused_control_;
	std::shared_ptr<Caret> caret_;

	CloseEvent close_event_;
};

}