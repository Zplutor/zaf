#pragma once

#include <Windows.h>
#include <zaf/base/event.h>
#include <zaf/base/flag_enum.h>
#include <zaf/base/property_map.h>
#include <zaf/control/control.h>
#include <zaf/graphic/rect.h>
#include <zaf/graphic/renderer.h>
#include <zaf/internal/message_loop.h>

namespace zaf {

class Caret;
class MouseMessage;

/**
 Represents a top-level window.

 You shoudl always use Create method to create a window.
 */
class Window : public std::enable_shared_from_this<Window> {
public:
    /**
     The initial rect style of a window.
     */
    enum class InitialRectStyle {

        /**
         The window is centered whithin the screen, and has a specified size set by
         the SetRect method.
         */
        CenterInScreen,

        /**
         The window is centered within its owner window, and has a specified size 
         set by the SetRect method.
         */
        CenterInOwner,

        /**
         The window's rect is specifed by the SetRect method.
         */
        Custom, 
    };

    /**
     The border style of a window.
     */
    enum class BorderStyle {

        /**
         The window has no border.
         */
        None,

        /**
         The window has a normal border.
         */
        Normal,

        /**
         The window has a dialog style border.
         */
        Dialog,

        /**
         The window has a tool window style border. 

         Windows with this style do not appear in the taskbar.
         */
        ToolWindow,
    };

    /**
     Specifies options when activating a window.

     This enumeration allows a bitwise combination of its member values.
     */
    enum class ActivateOption {

        /**
         The window is activated as normal.
         */
        None = 0,

        /**
         The window is not allow to be activated.

         A window with this value would not be shown in the taskbar.
         */
        NoActivate = 1,

        /**
         The mouse message that triggers the activating would be discard.
         */
        DiscardMouseMessage = 2,
    };

    /**
     The prototype of close handler.

     @param window
        The window instance which calls the close handler.

     @return 
        Return true means allowing to close the window; or false means
        not allowing.
     */
    typedef std::function<bool(const Window& window)> CloseHandler;

    /**
     The type of close event.
     */
	typedef Event<const std::shared_ptr<Window>&> CloseEvent;

public:
    /**
     Construct the instance.
     */
	Window();

    /**
     Destruct the instance.
     */
	virtual ~Window();

    /**
     Initialize the window.

     This method must be called after creating a window instance. 
     You can use the Create method to ensure that.
     */
    virtual void Initialize();

    /**
     Get the owner window.
     */
    const std::shared_ptr<Window> GetOwner() const;

    /**
     Set the owner window.

     This method takes effect only when the window is closed, otherwise the owner
     window is not changed.
     */
    void SetOwner(const std::shared_ptr<Window>& owner);

    /**
     Get the window's initial rect style.

     The default value is CenterInScreen.
     */
    InitialRectStyle GetInitialRectStyle() const;

    /**
     Set the window's initial rect style.
     */
    void SetInitialRectStyle(InitialRectStyle initial_rect_style);

    /**
     Get window's rect.

     This method returns the actual rect if the window has been created. Otherwise, 
     it returns the rect set by SetRect method. If SetRect has not been called, the 
     default rect (0, 0, 640, 480) is returned.
     */
    const Rect GetRect() const;

    /**
     Set window's rect.

     If the window has been created, this method would change the rect immediately.
     Otherwise, it just records the rect. When the window is being creatd, an appropriate
     rect would be set according to the initial rect style property.
     */
    void SetRect(const Rect& rect);

    /**
     Set window's size.

     This is a shortcut method for SetRect.

     See also SetRect.
     */
    void SetSize(const Size& size) {
        SetRect(Rect(GetRect().position, size));
    }

    /**
     Get window's client rect.
     */
    const Rect GetClientRect() const;

    /**
     Get window's activate option.

     The default option is None.
     */
    ActivateOption GetActivateOption() const;

    /**
     Set window's activate option.

     This method takes effect only when the window is closed, otherwise
     the option would not be changed.
     */
    void SetActivateOption(ActivateOption option);

    /**
     Get window's border style.

     The default border style is Normal.
     */
    BorderStyle GetBorderStyle() const;

    /**
     Set window's border style.

     This method takes effect only when the window is closed, otherwise 
     the style would not be changed.
     */
    void SetBorderStyle(BorderStyle border_style);

    /**
     Get a value indicating that whether the window is sizable.

     If the border style is None, this method always return false.
     For other border styles, the default value is true.
     */
    bool IsSizable() const;

    /**
     Set a value indicating that whether the window is sizable.

     This method takes effect only when the border style is not None.
     */
    void SetIsSizable(bool is_sizable);

    /**
     Get a value indicating that whether the window has the system menu.

     If the border style is None, this method always return false. For
     other border styles, the default value is true.
     */
    bool HasSystemMenu() const;

    /**
     Set a value indicating that whether the window has the system menu.

     This method takes effect only when the border style is not None.
     */
    void SetHasSystemMenu(bool has_system_menu);

    /**
     Get a value indicating that whether the window has a minimize button in title bar.

     If the window does not have the system menu, this method always return false.
     Otherwise, the default value is true.
     */
    bool HasMinimizeButton() const;

    /**
     Set a value indicating that whether the window has a minimize button in title bar.

     This method takes effect only when the window has the system menu.
     */
    void SetHasMinimizeButton(bool has_minimize_button);

    /**
     Get a value indicating that whether the window has a maximize button in title bar.

     If the window does not have the system menu, this method always return false.
     Otherwise, the default value is true.
     */
    bool HasMaximizeButton() const;

    /**
     Set a value indicating that whether the window has a maximize button in title bar.

     This method takes effect only when the window has the system menu.
     */
    void SetHasMaximizeButton(bool has_maximize_button);

    /**
     Get window's title.

     The default title is empty.
     */
    const std::wstring GetTitle() const;

    /**
     Set window's title.
     */
    void SetTitle(const std::wstring& title);

    /**
     Get window's root control.
     */
	const std::shared_ptr<Control>& GetRootControl() const {
		return root_control_;
	}

    /**
     Get the control which is capturing mouse in the window.
     */
    const std::shared_ptr<Control>& GetCapturingMouseControl() const {
        return capturing_mouse_control_;
    }

	/**
	 Get the control which has input focus in the window.
 	 */
	const std::shared_ptr<Control>& GetFocusedControl() const {
		return focused_control_;
	}

    /**
     Get the caret associates with the window.
     */
	const std::shared_ptr<Caret>& GetCaret();

    /**
     Get the renderer of the window.
     */
    Renderer& GetRenderer() {
        return renderer_;
    }

    /**
     Get the window's handle.
     */
    HWND GetHandle() const {
        return handle_;
    }

    /**
     Get a value indicating that whether the window is closed.
     */
    bool IsClosed() const {
        return GetHandle() == nullptr;
    }

    /**
     Get the close handler.

     If close handler is not set, the default one is return, which allows closing the window.
     */
    const CloseHandler GetCloseHandler() const;

    /**
     Set the close handler.

     The close handler is called before closing the window. You can use this handler to control
     whether the window is allowed to close.
     */
    void SetCloseHandler(const CloseHandler& handler);

    /**
     Get the close event.
     */
    CloseEvent::Proxy GetCloseEvent();

    /**
     Get position of the mouse cursor in current window's coordinate system.
     */
    const Point GetMousePosition() const;

    /**
     Show the window.
     */
	void Show();

    /**
     Hide the window.
     */
	void Hide();

    /**
     Close the window.
     */
	void Close();

protected:
    /**
     Preprocess a key message.

     @param message
         Contains information of the message.

     @return 
         Return true if the method processes the message, otherwise return false.

     This method is called before dispatching the WM_KEYDOWN and WM_SYSKEYDOWN messages.
     If the method returns true, the message would not been dispatched.

     Derived classes should always call the same method of super class if it doesn't 
     process the message.
     */
    virtual bool PreprocessMessage(const KeyMessage& message);

    /**
     This method is called when the window receives a message.

     @param message
        Contains information of the message.

     @param result
        An output parameter stores the result of handling the message.

     @return
        Return true if the method handles the message, otherwise return false.

     Derived classes should always call the same method of super class if it doesn't handle 
     the message.
     */
    virtual bool ReceiveMessage(const Message& message, LRESULT& result);

    /**
     This method is called after the window created.

     Dervied classes must call the same method of super class.
     */
    virtual void WindowCreate() { }

    /**
     This method is called after the window destroyed.

     @param handle
        The handle of the window has been destroyed.

     Derived classes must call the same method of super class.
     */
    virtual void WindowDestroy(HWND handle) { }

    /**
     This method is called after the window shown.

     Derived classes must call the same method of super class.
     */
    virtual void WindowShow() { }

    /**
     This method is called after the focused control changed.

     @param previous_focused_control
        The previous focused control, may be nullptr.

     Derived classes must call the same method of super class.
     */
    virtual void FocusedControlChange(const std::shared_ptr<Control>& previous_focused_control) { }

    /**
     This method is called after the capturing mouse control changed.

     @param previous_capture_control
         The previous capture mouse control, may be nullptr.

     Derived classes must call the same method of super class.
     */
    virtual void CapturingMouseControlChange(const std::shared_ptr<Control>& previous_capturing_control) { }

    /**
     Get the window's property map.
     */
    PropertyMap& GetPropertyMap() {
        return property_map_;
    }

    /**
     Get the window's property map.
     */
    const PropertyMap& GetPropertyMap() const {
        return property_map_;
    }

private:
	friend class Application;
	friend class Caret;
	friend class Control;
    friend class internal::MessageLoop;

	static void RegisterDefaultClass(std::error_code& error_code);

	void NeedRepaintRect(const Rect& rect);
	void SetHoveredControl(const std::shared_ptr<Control>& hovered_control);
	void SetCaptureMouseControl(const std::shared_ptr<Control>& capture_control, bool is_releasing);
	void SetFocusedControl(const std::shared_ptr<Control>& new_focused_control);

private:
	static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    void CreateWindowHandle();
    const Rect GetInitialRect() const;
    void CreateRenderer();
    void RecreateRenderer();
    void CheckCreateWindowHandle();
    void GetStyles(DWORD& style, DWORD& extract_style) const;

    void SwitchFocusedControlByTabKey(bool backward);

    void Repaint();
    void Resize(UINT width, UINT height);
    bool RedirectMouseWheelMessage(const Message& message);
    void ReceiveMouseMessage(const MouseMessage& message);
    bool ChangeMouseCursor(const Message& message);
    bool ReceiveCloseMessage();
    void ReceiveDestroyMessage();
	
    void CaptureMouseWithControl(const std::shared_ptr<Control>& control);
    void ReleaseMouseWithControl(const std::shared_ptr<Control>& control);

    bool GetStyleProperty(const std::wstring& property_name, DWORD style_value) const;
    void SetStyleProperty(const std::wstring& property_name, DWORD style_value, bool is_set);

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

private:
	HWND handle_;
    Rect rect_;
	Renderer renderer_;

	bool is_tracking_mouse_;

	std::shared_ptr<Control> root_control_;
	std::shared_ptr<Control> hovered_control_;
    std::shared_ptr<Control> capturing_mouse_control_;
	std::shared_ptr<Control> focused_control_;
	std::shared_ptr<Caret> caret_;

    PropertyMap property_map_;
};

ZAF_ENABLE_FLAG_ENUM(Window::ActivateOption);

const std::shared_ptr<Window> GetWindowFromHandle(HWND handle);

}