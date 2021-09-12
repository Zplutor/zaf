#pragma once

#include <Windows.h>
#include <zaf/control/control.h>
#include <zaf/graphic/rect.h>
#include <zaf/graphic/renderer/window_renderer.h>
#include <zaf/internal/message_loop.h>
#include <zaf/object/object.h>
#include <zaf/rx/subscription_host.h>
#include <zaf/serialization/property_map.h>
#include <zaf/window/activate_option.h>
#include <zaf/window/initial_rect_style.h>
#include <zaf/window/message/message.h>

namespace zaf {
namespace internal {
class InspectorPort;
}

class Caret;
class HitTestMessage;
class InspectorWindow;
class MouseMessage;
class WindowDestroyInfo;
class WindowReceiveMessageInfo;
enum class HitTestResult;

/**
 The prototype of close handler.

 @param window
    The window instance which calls the close handler.

 @return
    Return true means allowing to close the window; or false means
    not allowing.
 */
typedef std::function<bool(const Window& window)> WindowCloseHandler;


/**
 Represents a top-level window.

 You shoudl always use Create method to create a window.
 */
class Window : 
    public Object, 
    public SubscriptionHost, 
    public std::enable_shared_from_this<Window> {

public:
    ZAF_DECLARE_TYPE

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
    std::shared_ptr<Window> Owner() const;

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
    InitialRectStyle InitialRectStyle() const;

    /**
     Set the window's initial rect style.
     */
    void SetInitialRectStyle(zaf::InitialRectStyle initial_rect_style);

    /**
     Get window's rect.

     This method returns the actual rect if the window has been created. Otherwise, 
     it returns the rect set by SetRect method. If SetRect has not been called, the 
     default rect (0, 0, 640, 480) is returned.
     */
    Rect Rect() const;

    /**
     Set window's rect.

     If the window has been created, this method would change the rect immediately.
     Otherwise, it just records the rect. When the window is being creatd, an appropriate
     rect would be set according to the initial rect style property.
     */
    void SetRect(const zaf::Rect& rect);

    Point Position() const {
        return Rect().position;
    }

    void SetPosition(const Point& position) {
        SetRect(zaf::Rect{ position, Rect().size });
    }

    /**
     Get window's size.
     */
    Size Size() const {
        return Rect().size;
    }

    /**
     Set window's size.

     This is a shortcut method for SetRect.

     See also SetRect.
     */
    void SetSize(const zaf::Size& size) {
        SetRect(zaf::Rect(Rect().position, size));
    }

    void SetClientSize(const zaf::Size& size);

    /**
     Get window's minimum size.
     */
    zaf::Size MinSize() const {
        return zaf::Size(MinWidth(), MinHeight());
    }

    /**
     Set windows's minimum size.
     */
    void SetMinSize(const zaf::Size& size) {
        SetMinWidth(size.width);
        SetMinHeight(size.height);
    }

    /**
     Get windows's maximum size.
     */
    zaf::Size MaxSize() const {
        return zaf::Size(MaxWidth(), MaxHeight());
    }

    /**
     Set window's maximum size.
     */
    void SetMaxSize(const zaf::Size& size) {
        SetMaxWidth(size.width);
        SetMaxHeight(size.height);
    }

    /**
     Get window's width.
     */
    float Width() const {
        return Size().width;
    }

    /**
     Set window's width.
     */
    void SetWidth(float width) {
        SetSize(zaf::Size(width, Height()));
    }

    /**
     Get window's minimum width.
     */
    float MinWidth() const;

    /**
     Set window's minimum width.
     */
    void SetMinWidth(float min_width);

    /**
     Get window's maximum width.
     */
    float MaxWidth() const;

    /**
     Set window's maximum width.
     */
    void SetMaxWidth(float max_width);

    /**
     Get window's height.
     */
    float Height() const {
        return Size().height;
    }

    /**
     Set window's height.
     */
    void SetHeight(float height) {
        SetSize(zaf::Size(Width(), height));
    }

    /**
     Get window's minimum height.
     */
    float MinHeight() const;

    /**
     Set window's minimum height.
     */
    void SetMinHeight(float min_height);

    /**
     Get window's maximum height.
     */
    float MaxHeight() const;

    /**
     Set window's maximum height.
     */
    void SetMaxHeight(float max_height);

    /**
     Get window's client rect.
     */
    zaf::Rect ClientRect() const;

    /**
     Get window's activate option.

     The default option is None.
     */
    ActivateOption ActivateOption() const;

    /**
     Set window's activate option.

     This method takes effect only when the window is closed, otherwise
     the option would not be changed.
     */
    void SetActivateOption(zaf::ActivateOption option);

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
    std::wstring Title() const;

    /**
     Set window's title.
     */
    void SetTitle(const std::wstring& title);

    /**
     Get window's root control.
     */
    const std::shared_ptr<Control>& RootControl() const {
        return root_control_;
    }

    /**
     Set window's root control.
     */
    void SetRootControl(const std::shared_ptr<Control>& control);

    /**
     Get the control which is capturing mouse in the window.
     */
    const std::shared_ptr<Control>& CapturingMouseControl() const {
        return capturing_mouse_control_;
    }

    /**
     Get the control which is being hovered.
     */
    const std::shared_ptr<Control>& HoveredControl() const {
        return hovered_control_;
    }

    /**
     Get the control which has input focus in the window.
      */
    const std::shared_ptr<Control>& FocusedControl() const {
        return focused_control_;
    }

    /**
     Get the caret associates with the window.
     */
    const std::shared_ptr<Caret>& Caret();

    /**
     Get the renderer of the window.
     */
    Renderer& Renderer() {
        return renderer_;
    }

    /**
     Get the window's handle.
     */
    HWND Handle() const {
        return handle_;
    }

    /**
     Get a value indicating that whether the window is closed.
     */
    bool IsClosed() const {
        return Handle() == nullptr;
    }

    /**
     Get the close handler.

     If close handler is not set, the default one is return, which allows closing the window.
     */
    WindowCloseHandler CloseHandler() const;

    /**
     Set the close handler.

     The close handler is called before closing the window. You can use this handler to control
     whether the window is allowed to close.
     */
    void SetCloseHandler(const WindowCloseHandler& handler);

    /**
     Get the destroy event.
     */
    Observable<WindowDestroyInfo> DestroyEvent();

    Observable<WindowReceiveMessageInfo> ReceiveMessageEvent();

    /**
     Get position of the mouse cursor in current window's coordinate system.
     */
    Point GetMousePosition() const;

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

    void CreateHandle() {
        CheckCreateWindowHandle();
    }

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
    virtual void OnWindowDestroyed(HWND handle);

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

    static void RegisterDefaultClass(HICON icon, HICON small_icon);

    void NeedRepaintRect(const zaf::Rect& rect);
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
    zaf::Rect GetInitialRect() const;
    void CreateRenderer();
    void RecreateRenderer();
    void CheckCreateWindowHandle();
    void GetHandleStyles(DWORD& handle_style, DWORD& handle_extra_style) const;

    bool TryToPreprocessTabKeyMessage(const KeyMessage& message);
    void SwitchFocusedControlByTabKey(bool backward);
    bool TryToPreprocessInspectorShortcutMessage(const KeyMessage& message);

    void Repaint();
    void PaintInspectedControl(Canvas& canvas, const zaf::Rect& dirty_rect);
    bool ReceiveMessageEntrance(const Message& message, LRESULT& result);
    void RaiseReceiveMessageEvent(const Message& message, LRESULT result);
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
        bool is_extra_style);

    void SetStyleToHandle(
        DWORD style_value, 
        bool is_set,
        bool is_extra_style);

private:
    HWND handle_;
    zaf::Rect rect_;
    WindowRenderer renderer_;

    TrackMouseMode track_mouse_mode_{ TrackMouseMode::None };

    std::shared_ptr<Control> root_control_;
    std::shared_ptr<Control> hovered_control_;
    std::shared_ptr<Control> capturing_mouse_control_;
    std::shared_ptr<Control> focused_control_;
    std::shared_ptr<zaf::Caret> caret_;

    PropertyMap property_map_;

    std::weak_ptr<InspectorWindow> inspector_window_;
    std::shared_ptr<Control> highlight_control_;
    bool is_selecting_inspector_control_{};
};


class WindowDestroyInfo {
public:
    std::shared_ptr<Window> window;
};


class WindowReceiveMessageInfo {
public:
    std::shared_ptr<Window> window;
    Message message;
    LRESULT result{};
};


std::shared_ptr<Window> GetWindowFromHandle(HWND handle);

}