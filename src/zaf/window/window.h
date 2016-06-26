#pragma once

#include <Windows.h>
#include <zaf/base/event.h>
#include <zaf/base/property_map.h>
#include <zaf/control/control.h>
#include <zaf/graphic/rect.h>

namespace zaf {

class Caret;
class MouseMessage;
class Renderer;

class Window : public std::enable_shared_from_this<Window> {
public:
    typedef std::function<bool(const Window&)> CloseHandler;

	typedef Event<const std::shared_ptr<Window>&> CloseEvent;

public:
	Window();
	virtual ~Window();

    virtual void Initialize();

    const std::shared_ptr<Window> GetOwner() const;
    void SetOwner(const std::shared_ptr<Window>& owner);

    const Rect GetRect() const;
    void SetRect(const Rect& rect);

    const Rect GetClientRect() const;

    const std::wstring GetTitle() const;
    void SetTitle(const std::wstring& title);

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

    const std::shared_ptr<Renderer>& GetRenderer() const {
        return renderer_;
    }

    HWND GetHandle() const {
        return handle_;
    }

    bool IsClosed() const {
        return GetHandle() == nullptr;
    }

    const CloseHandler GetCloseHandler() const;
    void SetCloseHandler(const CloseHandler& handler);

    CloseEvent::Proxy GetCloseEvent();

    /**
     Get position of the mouse cursor in current window's coordinate system.
     */
    const Point GetMousePosition() const;

	void Show();
	void Hide();
	void Close();

protected:
    virtual void WindowCreate() { }
    virtual void WindowDestroy(HWND handle) { }

    virtual bool ReceiveMessage(const Message& message, LRESULT& result);

    PropertyMap& GetPropertyMap() {
        return property_map_;
    }

    const PropertyMap& GetPropertyMap() const {
        return property_map_;
    }

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

    void CreateWindowHandle();
    void CreateRenderer();
    void CheckCreateWindowHandle();

    void Repaint();
    void Resize(UINT width, UINT height);
    void ReceiveMouseMessage(const MouseMessage& message);
    void ReceiveKeyMessage(const KeyMessage& message);
    bool ChangeMouseCursor(const Message& message);
	void LostFocus();
    bool ReceiveCloseMessage();
    void ReceiveDestroyMessage();
	
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

private:
	HWND handle_;
    Rect rect_;
	std::shared_ptr<Renderer> renderer_;

	bool is_tracking_mouse_;
	bool is_capturing_mouse_;

	std::shared_ptr<Control> root_control_;
	std::shared_ptr<Control> hovered_control_;
	std::shared_ptr<Control> focused_control_;
	std::shared_ptr<Caret> caret_;

    PropertyMap property_map_;
};


}