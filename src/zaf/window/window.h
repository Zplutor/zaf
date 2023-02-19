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
#include <zaf/window/event/message_handled_info.h>
#include <zaf/window/event/message_received_info.h>
#include <zaf/window/initial_rect_style.h>
#include <zaf/window/message/message.h>
#include <zaf/window/window_event_infos.h>

namespace zaf {
namespace internal {
class InspectorPort;
class WindowClass;
}

class HitTestMessage;
class InspectorWindow;
class MouseMessage;
class TooltipWindow;
enum class HitTestResult;


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

    static constexpr const wchar_t* const DefaultClassName = L"ZafDef";

public:
    /**
     Construct the instance.
     */
    Window();
    Window(const std::wstring& window_class_name);

    /**
     Destruct the instance.
     */
    virtual ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    const std::wstring& ClassName() const;

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

    zaf::Size ContentSize() const;
    void SetContentSize(const zaf::Size& size);

    float ContentWidth() const {
        return ContentSize().width;
    }

    void SetContentWidth(float width) {
        SetContentSize(zaf::Size{ width, ContentSize().height });
    }

    float ContentHeight() const {
        return ContentSize().height;
    }

    void SetContentHeight(float height) {
        SetContentSize(zaf::Size{ ContentSize().width, height });
    }

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

    bool IsTopmost() const;
    void SetIsTopmost(bool is_topmost);

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
     Get the control which is under mouse cursor.
     */
    const std::shared_ptr<Control>& MouseOverControl() const {
        return mouse_over_control_;
    }

    /**
     Get the control which has input focus in the window.
      */
    const std::shared_ptr<Control>& FocusedControl() const {
        return focused_control_;
    }

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

    Observable<WindowCloseInfo> CloseEvent();

    /**
     Get the destroy event.
     */
    Observable<WindowDestroyInfo> DestroyEvent();

    Observable<MessageReceivedInfo> MessageReceivedEvent();
    Observable<MessageHandledInfo> MessageHandledEvent();

    /**
     Get position of the mouse cursor in current window's coordinate system.
     */
    Point GetMousePosition() const;

    float GetDPI() const;

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

    bool IsVisible() const;
    bool IsFocused() const;

    /**
     Close the window.
     */
    void Close();

    void Destroy();

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

    virtual void OnMessageReceived(const MessageReceivedInfo& event_info);
    virtual void OnMessageHandled(const MessageHandledInfo& event_info);

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
    friend class Control;
    friend class InspectorWindow;
    friend class internal::MessageLoop;
    friend class internal::WindowClass;

    void NeedRepaintRect(const zaf::Rect& rect);
    void SetMouseOverControl(
        const std::shared_ptr<Control>& mouse_over_control, 
        const MouseMessage& message);
    void ChangeControlMouseOverState(
        const std::shared_ptr<Control>& target_control,
        bool is_mouse_over);
    void SetCaptureMouseControl(
        const std::shared_ptr<Control>& capture_control,
        bool is_releasing);
    void SetFocusedControl(const std::shared_ptr<Control>& new_focused_control);
    void ChangeControlFocusState(
        const std::shared_ptr<Control>& target_control, 
        const std::shared_ptr<Control>& changing_control,
        bool is_focused);

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
    LRESULT HandleWMCREATE(const Message& message);
    std::optional<LRESULT> HandleWMNCCALCSIZE(const Message& message);
    zaf::Rect GetInitialRect(float dpi) const;
    void CreateRenderer();
    void RecreateRenderer();
    void CheckCreateWindowHandle();
    void GetHandleStyles(DWORD& handle_style, DWORD& handle_extra_style) const;
    zaf::Size AdjustContentSizeToWindowSize(const zaf::Size& content_size) const;

    bool TryToPreprocessTabKeyMessage(const KeyMessage& message);
    void SwitchFocusedControlByTabKey(bool backward);
    bool TryToPreprocessInspectorShortcutMessage(const KeyMessage& message);

    LRESULT RouteWindowMessage(HWND hwnd, UINT id, WPARAM wparam, LPARAM lparam);
    void RaiseMessageReceivedEvent(const MessageReceivedInfo& event_info);
    std::optional<LRESULT> HandleMessage(const Message& message);

    void HandleWMPAINT();
    void PaintInspectedControl(Canvas& canvas, const zaf::Rect& dirty_rect);
    void Resize(UINT width, UINT height);
    void HandleSizeMessage(const Message& message);
    void HandleMoveMessage();
    void UpdateWindowRect();
    bool RedirectMouseWheelMessage(const Message& message);
    bool HandleMouseMessage(const MouseMessage& message);
    std::shared_ptr<Control> GetBeginRoutingControlForMouseMessage(
        const MouseMessage& message,
        Point& position_at_control) const;
    bool RouteMouseEvent(
        const std::shared_ptr<Control>& begin_routing_control,
        const Point& position_at_begin_routing_control,
        const MouseMessage& message);
    void HighlightControlAtPosition(const Point& position);
    void SelectInspectedControl();
    void TrackMouseByMouseMove(const MouseMessage& message);
    void TrackMouse(bool is_non_client);
    void OnMouseHover(const Message& message);
    void OnMouseLeave(const Message& message);
    bool OnKeyDown(const Message& message);
    bool OnKeyUp(const Message& message);
    bool OnCharInput(const Message& message);
    void TryToShowTooltipWindow();
    void HideTooltipWindow();
    bool ChangeMouseCursor(const Message& message);
    bool HandleCloseMessage();
    void HandleDestroyMessage();
    
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
    std::shared_ptr<internal::WindowClass> class_;
    HWND handle_{};
    zaf::Rect rect_{ 0, 0, 640, 480 };
    WindowRenderer renderer_;

    TrackMouseMode track_mouse_mode_{ TrackMouseMode::None };

    std::shared_ptr<Control> root_control_;
    std::shared_ptr<Control> mouse_over_control_;
    std::shared_ptr<Control> capturing_mouse_control_;
    std::shared_ptr<Control> focused_control_;
    std::weak_ptr<Control> last_focused_control_;
    std::shared_ptr<TooltipWindow> tooltip_window_;

    PropertyMap property_map_;

    std::weak_ptr<InspectorWindow> inspector_window_;
    std::shared_ptr<Control> highlight_control_;
    bool is_selecting_inspector_control_{};
};


std::shared_ptr<Window> GetWindowFromHandle(HWND handle);

}