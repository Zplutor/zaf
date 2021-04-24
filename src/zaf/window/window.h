#pragma once

#include <Windows.h>
#include <zaf/base/flag_enum.h>
#include <zaf/control/control.h>
#include <zaf/graphic/rect.h>
#include <zaf/graphic/renderer/window_renderer.h>
#include <zaf/internal/message_loop.h>
#include <zaf/reflection/reflection_object.h>
#include <zaf/rx/subscription_host.h>
#include <zaf/serialization/property_map.h>

namespace zaf {
namespace internal {
class InspectorPort;
}

class Caret;
class HitTestMessage;
class InspectorWindow;
class MouseMessage;
class WindowCloseInfo;
enum class HitTestResult;

/**
 Represents a top-level window.

 You shoudl always use Create method to create a window.
 */
class Window : 
    public ReflectionObject, 
    public SubscriptionHost, 
    public std::enable_shared_from_this<Window> {

public:
    ZAF_DECLARE_REFLECTION_TYPE

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

public:
    /**
     Construct the instance.
     */
	Window();

    /**
     Destruct the instance.
     */
	virtual ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    /**
     Get the owner window.
     */
    std::shared_ptr<Window> GetOwner() const;

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
    Rect GetRect() const;

    /**
     Set window's rect.

     If the window has been created, this method would change the rect immediately.
     Otherwise, it just records the rect. When the window is being creatd, an appropriate
     rect would be set according to the initial rect style property.
     */
    void SetRect(const Rect& rect);

    /**
     Get window's size.
     */
    Size GetSize() const {
        return GetRect().size;
    }

    /**
     Set window's size.

     This is a shortcut method for SetRect.

     See also SetRect.
     */
    void SetSize(const Size& size) {
        SetRect(Rect(GetRect().position, size));
    }

    /**
     Get window's minimum size.
     */
    Size GetMinimumSize() const {
        return Size(GetMinimumWidth(), GetMinimumHeight());
    }

    /**
     Set windows's minimum size.
     */
    void SetMinimumSize(const Size& size) {
        SetMinimumWidth(size.width);
        SetMinimumHeight(size.height);
    }

    /**
     Get windows's maximum size.
     */
    Size GetMaximumSize() const {
        return Size(GetMaximumWidth(), GetMaximumHeight());
    }

    /**
     Set window's maximum size.
     */
    void SetMaximumSize(const Size& size) {
        SetMaximumWidth(size.width);
        SetMaximumHeight(size.height);
    }

    /**
     Get window's width.
     */
    float GetWidth() const {
        return GetSize().width;
    }

    /**
     Set window's width.
     */
    void SetWidth(float width) {
        SetSize(zaf::Size(width, GetHeight()));
    }

    /**
     Get window's minimum width.
     */
    float GetMinimumWidth() const;

    /**
     Set window's minimum width.
     */
    void SetMinimumWidth(float min_width);

    /**
     Get window's maximum width.
     */
    float GetMaximumWidth() const;

    /**
     Set window's maximum width.
     */
    void SetMaximumWidth(float max_width);

    /**
     Get window's height.
     */
    float GetHeight() const {
        return GetSize().height;
    }

    /**
     Set window's height.
     */
    void SetHeight(float height) {
        SetSize(zaf::Size(GetWidth(), height));
    }

    /**
     Get window's minimum height.
     */
    float GetMinimumHeight() const;

    /**
     Set window's minimum height.
     */
    void SetMinimumHeight(float min_height);

    /**
     Get window's maximum height.
     */
    float GetMaximumHeight() const;

    /**
     Set window's maximum height.
     */
    void SetMaximumHeight(float max_height);

    /**
     Get window's client rect.
     */
    Rect GetClientRect() const;

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
     Get a value indicating that whether the window is a popup window.

     The default value is false, means that the window is an overlapped window.
     */
    bool IsPopup() const;

    /**
     Set a value indicating that whether the window is a popup window.

     This method takes effect only when the window is closed, otherwise the style would not change.
     */
    void SetIsPopup(bool is_popup);

    /**
     Get a value indicating that whether the window has border.

     The default value is true.
     */
    bool HasBorder() const;

    /**
     Set a value indicating that whether the window has border.

     This method takes effect only when the window is closed, otherwise the border would not change.
    */
    void SetHasBorder(bool has_border);

    /**
     Get a value indicating that whether the window has title bar.

     The default value is true. For overlapped window that has border, this property is always true. 
     */
    bool HasTitleBar() const;

    /**
     Set a value indicating that whether the window has title bar.

     For overlapped window that has border, setting this property takes no effects. If HasBorder() 
     is false, setting this property takes no visual effects.
     */
    void SetHasTitleBar(bool has_title_bar);

    /**
     Get a value indicating that whether the window is sizable.

     The default value is true.
     */
    bool IsSizable() const;

    /**
     Set a value indicating that whether the window is sizable.

     If HasBorder() is false, setting this property takes no visual effects.
     */
    void SetIsSizable(bool is_sizable);

    /**
     Get a value indicating that whether the window has the system menu.

     The default value is true.
     */
    bool HasSystemMenu() const;

    /**
     Set a value indicating that whether the window has the system menu.

     If HasBorder is false, setting this property takes no visual effects.
     */
    void SetHasSystemMenu(bool has_system_menu);

    /**
     Get a value indicating that whether the window can be minimized.

     The default value is true.
     */
    bool CanMinimize() const;

    /**
     Set a value indicating that whether the window can be minimized.

     In order to display the minimize button in title bar, HasSystemMenu needs to be set to true 
     as well. If HasBorder is false, setting this property takes no visual effects. 
     */
    void SetCanMinimize(bool can_minimize);

    /**
     Get a value indicating that whether the window can be maximized.

     The default value is true.
     */
    bool CanMaximize() const;

    /**
     Set a value indicating that whether the window can be maximized.

     In order to display the maximize button in title bar, HasSystemMenu needs to be set to true 
     as well. If HasBorder is false, setting this property takes no visual effects. However, setting
     this property to true is useful in some cases. For example, it enables maximizing a custom 
     painted window when double clicking on a area that returns title bar on hit test.
     */
    void SetCanMaximize(bool can_maximize);

    /**
     Get a value indicating that whether the window is a tool window.

     The default value is false.
     */
    bool IsToolWindow() const;

    /**
     Set a value indicating that whether the window is a tool window.
     */
    void SetIsToolWindow(bool is_tool_window);

    /**
     Get window's title.

     The default title is empty.
     */
    std::wstring GetTitle() const;

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
     Set window's root control.
     */
    void SetRootControl(const std::shared_ptr<Control>& control);

    /**
     Get the control which is capturing mouse in the window.
     */
    const std::shared_ptr<Control>& GetCapturingMouseControl() const {
        return capturing_mouse_control_;
    }

    /**
     Get the control which is being hovered.
     */
    const std::shared_ptr<Control>& GetHoveredControl() const {
        return hovered_control_;
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
    Observable<WindowCloseInfo> CloseEvent();

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

    void Maximize();
    void Minimize();
    void Restore();

    /**
     Close the window.
     */
	void Close();

    void ShowInspectorWindow();

protected:
    void Initialize() override;

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

    virtual std::optional<HitTestResult> HitTest(const HitTestMessage& message);

    /**
     This method is called after the window created.

     Dervied classes must call the same method of super class.
     */
    virtual void OnWindowCreated() { }

    /**
     This method is called after the window destroyed.

     @param handle
        The handle of the window has been destroyed.

     Derived classes must call the same method of super class.
     */
    virtual void OnWindowDestroyed(HWND handle) { }

    /**
     This method is called after the window shown.

     Derived classes must call the same method of super class.
     */
    virtual void OnWindowShown() { }

    virtual void OnRootControlChanged(const std::shared_ptr<Control>& previous_root_control) { } 

    /**
     This method is called after the focused control changed.

     @param previous_focused_control
        The previous focused control, may be nullptr.

     Derived classes must call the same method of super class.
     */
    virtual void OnFocusedControlChanged(
        const std::shared_ptr<Control>& previous_focused_control) { }

    /**
     This method is called after the capturing mouse control changed.

     @param previous_capture_control
         The previous capture mouse control, may be nullptr.

     Derived classes must call the same method of super class.
     */
    virtual void OnCapturingMouseControlChanged(
        const std::shared_ptr<Control>& previous_capturing_control) { }

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
    friend class InspectorWindow;
    friend class internal::MessageLoop;

	static void RegisterDefaultClass();

	void NeedRepaintRect(const Rect& rect);
	void SetHoveredControl(
        const std::shared_ptr<Control>& hovered_control, 
        const MouseMessage& message);
	void SetCaptureMouseControl(const std::shared_ptr<Control>& capture_control, bool is_releasing);
	void SetFocusedControl(const std::shared_ptr<Control>& new_focused_control);

    void SetHighlightControl(const std::shared_ptr<Control>& inspected_control);
    std::shared_ptr<internal::InspectorPort> GetInspectorPort() const;
    void BeginSelectInspectedControl();

private:
    enum class TrackMouseMode {
        None,
        ClientArea,
        NonClientArea,
    };

private:
	static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

private:
    void InitializeRootControl(const std::shared_ptr<Control>& control);
    void CreateWindowHandle();
    const Rect GetInitialRect() const;
    void CreateRenderer();
    void RecreateRenderer();
    void CheckCreateWindowHandle();
    void GetHandleStyles(DWORD& handle_style, DWORD& handle_extract_style) const;

    bool TryToPreprocessTabKeyMessage(const KeyMessage& message);
    void SwitchFocusedControlByTabKey(bool backward);
    bool TryToPreprocessInspectorShortcutMessage(const KeyMessage& message);

    void Repaint();
    void PaintInspectedControl(Canvas& canvas, const Rect& dirty_rect);
    void Resize(UINT width, UINT height);
    bool RedirectMouseWheelMessage(const Message& message);
    bool ReceiveMouseMessage(const MouseMessage& message);
    void HighlightControlAtPosition(const Point& position);
    void SelectInspectedControl();
    void TrackMouseLeave(const MouseMessage& message);
    void OnMouseLeave(const MouseMessage& message);
    bool ChangeMouseCursor(const Message& message);
    bool ReceiveCloseMessage();
    void ReceiveDestroyMessage();
	
    void CaptureMouseWithControl(const std::shared_ptr<Control>& control);
    void ReleaseMouseWithControl(const std::shared_ptr<Control>& control);

    void ReviseHasTitleBar();

    void SetStyleProperty(
        const std::wstring& property_name,
        DWORD style_value,
        bool is_set,
        bool is_extract_style);

    void SetStyleToHandle(
        DWORD style_value, 
        bool is_set,
        bool is_extract_style);

private:
	HWND handle_;
    Rect rect_;
	WindowRenderer renderer_;

    TrackMouseMode track_mouse_mode_{ TrackMouseMode::None };

	std::shared_ptr<Control> root_control_;
	std::shared_ptr<Control> hovered_control_;
    std::shared_ptr<Control> capturing_mouse_control_;
	std::shared_ptr<Control> focused_control_;
	std::shared_ptr<Caret> caret_;

    PropertyMap property_map_;

    std::weak_ptr<InspectorWindow> inspector_window_;
    std::shared_ptr<Control> highlight_control_;
    bool is_selecting_inspector_control_{};
};


ZAF_ENABLE_FLAG_ENUM(Window::ActivateOption);


class WindowCloseInfo {
public:
    std::shared_ptr<Window> window;
};

std::shared_ptr<Window> GetWindowFromHandle(HWND handle);

}