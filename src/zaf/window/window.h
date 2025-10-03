#pragma once

#include <Windows.h>
#include <optional>
#include <zaf/base/event/event.h>
#include <zaf/base/non_copyable.h>
#include <zaf/base/none.h>
#include <zaf/control/control.h>
#include <zaf/graphic/rect.h>
#include <zaf/graphic/d2d/window_renderer.h>
#include <zaf/internal/message_loop.h>
#include <zaf/object/object.h>
#include <zaf/object/property_support.h>
#include <zaf/rx/subject/single_subject.h>
#include <zaf/rx/disposable_host.h>
#include <zaf/window/activate_option.h>
#include <zaf/window/event/activate_event_info.h>
#include <zaf/window/event/closing_info.h>
#include <zaf/window/event/destroyed_info.h>
#include <zaf/window/event/focused_control_changed_info.h>
#include <zaf/window/event/handle_created_info.h>
#include <zaf/window/event/message_handled_info.h>
#include <zaf/window/event/message_received_info.h>
#include <zaf/window/event/mouse_capture_control_changed_info.h>
#include <zaf/window/event/root_control_changed_info.h>
#include <zaf/window/event/show_window_event_info.h>
#include <zaf/window/event/window_focus_event_info.h>
#include <zaf/window/event/window_size_changed_info.h>
#include <zaf/window/initial_rect_style.h>
#include <zaf/window/message/message.h>
#include <zaf/window/window_handle_state.h>
#include <zaf/window/window_messager.h>
#include <zaf/window/window_parser.h>

namespace zaf {
namespace internal {
class InspectorPort;
class WindowFocusedControlManager;
}

class HitTestMessage;
class InspectorWindow;
class MouseMessage;
class TooltipWindow;
class WindowClass;
class WindowClassRegistry;
class WindowHolder;
enum class HitTestResult;


/**
 Represents a top-level window.

 You should always use Create method to create a window.
 */
class Window : 
    public Object, 
    public rx::DisposableHost,
    public std::enable_shared_from_this<Window>,
    NonCopyableNonMovable {

public:
    ZAF_OBJECT;

public:
    /**
     Construct the instance.
     */
    Window();
    explicit Window(const std::wstring& window_class_name);
    explicit Window(const std::shared_ptr<WindowClass>& window_class);

    /**
     Destruct the instance.
     */
    virtual ~Window();

    const std::shared_ptr<WindowClass>& Class() const {
        return class_;
    }

    /**
    Gets the window handle.
    */
    HWND Handle() const noexcept {
        return handle_;
    }

    /**
    Gets the state of the window handle.
    */
    WindowHandleState HandleState() const noexcept {
        return handle_state_;
    }

    /**
    Creates the window handle explicitly.

    @return
        A `zaf::WindowHolder` instance that keeps the handle alive. The window handle will be 
        destroyed when the holder is destructed.

    @post
        The return value is not null.

    @throw std::bad_alloc

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state isn't `NotCreated` nor `Created`.

    @throw zaf::Win32Error
        Thrown if fails to create the window handle.

    @throw ...
        Any exception that may be thrown during the window handle creation.

    @details
        There are three steps during the creation of the window handle:
        1. Create the window handle via Win32 API.
        2. Initialize resources related to the window handle, such as the renderer.
        3. Call the `OnHandleCreated()` method to raise the `HandleCreatedEvent()`, notify derived 
           classes and observers to do their initialization.

        These three steps are regarded as a single atomic operation. If any exception is thrown 
        during the operation, anything that has been done will be rolled back, including the 
        following:
        1. Destroy the window handle if it has been created.
        2. Release resources that have been initialized.
        3. Call the `OnDestroyed()` method to raise the `DestroyedEvent()` if the window handle has 
           been created.

        The window handle state will be reset to `NotCreated` if the operation fails, so that this
        method can be called again.

        If the window handle has been created, the existing holder will be returned. Users must
        keep the holder during the usage of the window handle.
        
        If the window handle is destroyed before the holder destructed, the existing holder will be
        detached.

    @see zaf::WindowHandleState
    @see zaf::WindowHolder
    @see zaf::Window::Show()
    */
    [[nodiscard]]
    std::shared_ptr<WindowHolder> CreateHandle();

    /**
    Gets the event that is raised after the window handle is created.

    @details
        If any exception is thrown from observers of the event, the window handle will be
        destroyed.

    @see zaf::Window::CreateHandle()
    */
    rx::Observable<HandleCreatedInfo> HandleCreatedEvent() const;

    /**
    Destroys the window handle.
    */
    void Destroy() noexcept;

    /**
    Gets the event that is raised when the window handle is destroyed.
    */
    rx::Observable<DestroyedInfo> DestroyedEvent() const;

    /**
    Shows the window, creates the window handle is it has not been created.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state isn't `NotCreated`, `Creating` nor `Created`.

    @throw ...
        Any exception thrown by the `CreateHandle()` method if fails to create the window handle.

    @details
        This method will call the `CreateHandle()` method implicitly to create the window handle.
        The returned holder of the `CreateHandle()` method is registered to the application, so 
        users don't need to keep the window or the holder by themselves. When the window handle is 
        destroyed, its holder will be unregistered from the application, so that the window 
        instance will be destructed as well.
    */
    void Show();

    /**
    Hides the window.

    @details
        This method takes effect only when the window handle state is `Created`, otherwise it does 
        nothing. The window is remained registered in the application after it is hidden.
    */
    void Hide();

    /**
     Get the owner window.
     */
    std::shared_ptr<Window> Owner() const noexcept;

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

    zaf::Rect ContentRect() const;

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

    rx::Observable<WindowSizeChangedInfo> SizeChangedEvent() const;

    /**
    Gets a single that emits when the window has exited both the sizing and moving states.

    @details
        The returned single emits immediately if the window is not in a sizing or moving state when
        calling this method.

    @return
        A single that emits only `OnSuccess` and won't emit `OnError`.

    @throw std::bad_alloc
    */
    rx::Single<None> WhenNotSizingOrMoving() const;

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
    Sets specified control as the root control of the window.

    @param control
        The new root control. It can't be nullptr, can't be the root control of another window, and
        can't have parent.

    @throw std::logic_error
        Thrown if control doesn't meet the precondiction.
    */
    void SetRootControl(const std::shared_ptr<Control>& control);

    rx::Observable<RootControlChangedInfo> RootControlChangedEvent() const;

    /**
     Gets the control which captures mouse in the window.
     */
    const std::shared_ptr<Control>& MouseCaptureControl() const {
        return mouse_capture_control_;
    }

    /**
    Gets mouse capture control changed event. This event is raised after the mouse capture control 
    changed.
    */
    rx::Observable<MouseCaptureControlChangedInfo> MouseCaptureControlChangedEvent() const;

    /**
     Get the control which is under mouse cursor.
     */
    const std::shared_ptr<Control>& MouseOverControl() const {
        return mouse_over_control_;
    }

    /**
     Get the control which has input focus in the window.
      */
    std::shared_ptr<Control> FocusedControl() const;

    rx::Observable<FocusedControlChangedInfo> FocusedControlChangedEvent() const;

    /**
     Get the renderer of the window.
     */
    d2d::Renderer& Renderer() {
        return renderer_;
    }

    /**
    Window show event. This event is raised when the window receives WM_SHOWWINDOW message, whose
    wParam is TRUE.
    */
    rx::Observable<ShowInfo> ShowEvent() const;

    /**
    Window hide event. This event is raised when the window receives WM_SHOWWINDOW message, whose
    wParam is FALSE.
    */
    rx::Observable<HideInfo> HideEvent() const;

    /**
    Window activated event. This event is raised when the window receives WM_ACTIVATE message, 
    whose wParam is WA_ACTIVE or WA_CLICKACTIVE.
    */
    rx::Observable<ActivatedInfo> ActivatedEvent() const;

    /**
    Window deactivated event. This event is raised when the window receives WM_ACTIVATE message,
    whose wParam is WA_INACTIVE.
    */
    rx::Observable<DeactivatedInfo> DeactivatedEvent() const;

    /**
    Window gained focus event. This event is raised when the window receives WM_SETFOCUS message.
    */
    rx::Observable<WindowFocusGainedInfo> FocusGainedEvent() const;

    /**
    Window lost focus event. This event is raised when the window receives WM_KILLFOCUS message.
    */
    rx::Observable<WindowFocusLostInfo> FocusLostEvent() const;

    rx::Observable<ClosingInfo> ClosingEvent() const;

    rx::Observable<MessageReceivedInfo> MessageReceivedEvent() const;
    rx::Observable<MessageHandledInfo> MessageHandledEvent() const;

    /**
    Gets the messager to send or post messages to the window.

    @return 
        The messager object. 

    @throw std::logic_error
        Thrown if the window handle is not created yet.
    */
    WindowMessager Messager();

    /**
     Get position of the mouse cursor in current window's coordinate system.
     */
    Point GetMousePosition() const;

    /**
    Translates a position from the window's client area to screen coordinates.

    @position
        The position in the window's client area coordinates.

    @return
        The translated position in screen coordinates.

    @throw std::logic_error
        Thrown if the window handle hasn't been created yet.
    */
    Point TranslateToScreen(const Point& position) const;

    /**
    Translates a position from the screen coordinates to the window's client area coordinates.

    @position
        The position in the screen coordinates.

    @return 
        The translated position in window's client area coordinates.

    @throw std::logic_error
        Thrown if the window handle hasn't been created yet.
    */
    Point TranslateFromScreen(const Point& position) const;

    float GetDPI() const;

    /**
    Attempts to bring the window to the foreground and activates it.

    @pre
        The window handle has been created.

    @return
        Returns true if the window is activated; otherwise false.

    @throw zaf::PreconditionError

    @details
        This method calls `SetForegroundWindow` to activate the window.
    */
    bool Activate();

    void Maximize();
    void Minimize();
    void Restore();

    bool IsVisible() const noexcept;
    bool IsFocused() const;

    /**
     Close the window.
     */
    void Close();

    void ShowInspectorWindow();

protected:
    void Initialize() override;

    /**
    Called after the window handle is created.

    @param event_info
        Information of the event.

    @details
        The default implementation of this method raises the `HandleCreatedEvent()`. Derived 
        classes should call the same method of the base class if they override this method.

        If any exception is thrown from this method, the window handle will be destroyed.

    @see zaf::Window::CreateHandle()
     */
    virtual void OnHandleCreated(const HandleCreatedInfo& event_info);

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
    Handle message received event. This method is called when the window receives a message, but 
    before passing it to the default window procedure.

    @param event_info
        Information of the event. Call event_info.MarkAsHandled() to prevent the message from being
        passed to the default window procedure.

    The default implementation of this method will raise MessageReceivedEvent first, and then do 
    some default handling according to different message. Event handlers might call 
    MarkAsHandle() as well, in such case the default handling wouldn't be called. 

    Derived classes must call the same method of base class, unless they have special needs to 
    change default handling of the message.
    */
    virtual void OnMessageReceived(const MessageReceivedInfo& event_info);

    /**
    Handle message handled event. This method is called after the message is handled, including the 
    default window procedure.

    @param event_info
        Information of the event.

    The default implementation of this method will raise MessageHandledEvent. Derived classes 
    should call the same method of base class.
    */
    virtual void OnMessageHandled(const MessageHandledInfo& event_info);

    virtual std::optional<HitTestResult> HitTest(const HitTestMessage& message);

    /**
    Handles window show event. This method is called when the window receives WM_SHOWWINDOW 
    message, whose wParam is TRUE.

    @param event_info
        Information of the event.

    The default implementation raises ShowEvent. Derived classes should call the same method of 
    base class.
    */
    virtual void OnShow(const ShowInfo& event_info);

    /**
    Handles window hide event. This method is called when the window receives WM_SHOWWINDOW 
    message, whose wParam is FALSE.

    @param event_info
        Information of the event.

    The default implementation raises HideEvent. Derived classes should call the same method of
    base class.
    */
    virtual void OnHide(const HideInfo& event_info);

    /**
    Handles window size changed event. This method is called when the window receives WM_SIZE
    message.

    @param event_info
        Information of the event.

    The default implementation raises SizeChangedEvent. Derived classes should call the same method
    of base class.
    */
    virtual void OnSizeChanged(const WindowSizeChangedInfo& event_info);

    /**
    Handles window activated event. This event is raised when the window receives WM_ACTIVATE
    message, whose wParam is WA_ACTIVE or WA_CLICKACTIVE.

    @param event_info
        Information of the event.

    The default implementation raises WindowActivatedEvent. Derived class should call the same 
    method of base class.
    */
    virtual void OnActivated(const ActivatedInfo& event_info);

    /**
    Handles window deactivated event. This event is raised when the window receives WM_ACTIVATE
    message, whose wParam is WA_INACTIVE.

    @param event_info
        Information of the event.

    The default implementation raises WindowDeactivatedEvent. Derived class should call the same
    method of base class.
    */
    virtual void OnDeactivated(const DeactivatedInfo& event_info);

    /**
    Handles window focus gained event. This method is called when the window receives WM_SETFOCUS
    message.

    @param event_info
        Information of the event.

    The default implementation raises FocusGainedEvent. Derived classes should call the same method
    of base class.
    */
    virtual void OnFocusGained(const WindowFocusGainedInfo& event_info);

    /**
    Handles window focus lost event. This method is called when the window receives WM_KILLFOCUS
    message.

    @param event_info
        Information of the event.

    The default implementation raises FocusLostEvent. Derived classes should call the same method
    of base calss.
    */
    virtual void OnFocusLost(const WindowFocusLostInfo& event_info);

    /**
    Handles window closing event. This method is called when the window receives WM_CLOSE message.

    @param event_info
        Information of the event. Call event_info.SetCanClose() to indicate that whether the window
        is allowed to close.

    The default implementation of this method raises ClosingEvent. Derived classes should call the
    same method of base class.
    */
    virtual void OnClosing(const ClosingInfo& event_info);

    /**
    Handles window destroyed event. This method is called after the window handles WM_DESTROY 
    message.

    @param event_info
        Information of the event.

    The default implementation of this method raises DestroyedEvent. Derived classes should
    call the same method of base class.
    */
    virtual void OnDestroyed(const DestroyedInfo& event_info) noexcept;

    virtual void OnRootControlChanged(const RootControlChangedInfo& event_info);

    /**
    Handle focused control changed event. This method is called after the focused control is 
    changed.

    @param event_info
        Information of the event.

    The default implementation of this method raises FocusedControlChangedEvent. Derived classes
    should call the same method of base class.
    */
    virtual void OnFocusedControlChanged(const FocusedControlChangedInfo& event_info);

    /**
    Handles mouse capture changed event. This method is called after the mouse capture control
    changed.

    The default implementation raise MouseCaptureChangedEvent. Derived classes should call the same 
    method of base class.
    */
    virtual void OnMouseCaptureControlChanged(const MouseCaptureControlChangedInfo& event_info);

private:
    friend class Application;
    friend class Control;
    friend class InspectorWindow;
    friend class internal::MessageLoop;
    friend class WindowClassRegistry;

    void NeedRepaintRect(const zaf::Rect& rect);
    void SetMouseOverControl(
        const std::shared_ptr<Control>& mouse_over_control, 
        const MouseMessage& message);
    void ChangeControlMouseOverState(
        const std::shared_ptr<Control>& target_control,
        bool is_mouse_over,
        const std::shared_ptr<Control>& changed_control);
    void SetMouseCaptureControl(
        const std::shared_ptr<Control>& capture_control,
        bool is_releasing);
    void CancelMouseCapture();

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
    void InnerCreateHandle();
    LRESULT HandleWMCREATE(const Message& message);
    std::optional<LRESULT> HandleWMNCCALCSIZE(const Message& message);
    zaf::Rect GetInitialRect(float dpi) const;
    void CreateRenderer();
    void RecreateRenderer();
    void GetHandleStyles(DWORD& handle_style, DWORD& handle_extra_style) const;
    zaf::Size AdjustContentSizeToWindowSize(const zaf::Size& content_size) const;

    bool TryToPreprocessInspectorShortcutMessage(const KeyMessage& message);

    LRESULT RouteWindowMessage(HWND hwnd, UINT id, WPARAM wparam, LPARAM lparam);
    void RaiseMessageReceivedEvent(const MessageReceivedInfo& event_info);
    std::optional<LRESULT> HandleMessage(const Message& message);

    void HandleWMPAINT();
    void PaintInspectedControl(Canvas& canvas, const zaf::Rect& dirty_rect);
    void Resize(UINT width, UINT height);
    void HandleWMSHOWWINDOW(const ShowWindowMessage& message);
    void HandleWMACTIVATE(const ActivateMessage& message);
    void HandleWMSETFOCUS(const SetFocusMessage& message);
    void HandleWMKILLFOCUS(const KillFocusMessage& message);
    void HandleWMSIZEMessage(const Message& message);
    void HandleMoveMessage();
    void UpdateWindowRect();
    bool RedirectMouseWheelMessage(const Message& message);
    bool HandleMouseMessage(const MouseMessage& message);
    bool RouteMouseEvent(const MouseMessage& message);
    std::shared_ptr<Control> GetBeginRoutingControlForMouseMessage(
        const MouseMessage& message,
        Point& position_at_control) const;
    void HighlightControlAtPosition(const Point& position);
    void SelectInspectedControl();
    void TrackMouseByMouseMove(const MouseMessage& message);
    void TrackMouse(bool is_non_client);
    void OnMouseHover(const Message& message);
    void OnMouseLeave(const Message& message);
    bool HandleKeyboardMessage(const Message& message);
    void TryToShowTooltipWindow();
    void HideTooltipWindow();
    bool HandleWMSETCURSOR(const Message& message);
    void HandleIMEMessage(const Message& message);
    void HandleWMCLOSE();
    void HandleWMDESTROY() noexcept;
    void HandleWMNCDESTROY() noexcept;
    
    void CaptureMouseWithControl(const std::shared_ptr<Control>& control);
    void ReleaseMouseWithControl(const std::shared_ptr<Control>& control);

    void ReviseHasTitleBar();

    void SetStyleProperty(
        bool& property_value,
        DWORD style_value,
        bool is_set,
        bool is_extra_style);

    void SetStyleToHandle(
        DWORD style_value, 
        bool is_set,
        bool is_extra_style);

private:
    std::shared_ptr<WindowClass> class_;
    HWND handle_{};
    WindowHandleState handle_state_{ WindowHandleState::NotCreated };
    DestroyReason destroy_reason_{ DestroyReason::Unspecified };
    std::weak_ptr<WindowHolder> holder_;
    zaf::Rect rect_{ 0, 0, 640, 480 };
    d2d::WindowRenderer renderer_;

    struct {
        bool is_sizing_or_moving{};
        mutable std::optional<rx::SingleSubject<zaf::None>> exit_sizing_or_moving_subject;
    } handle_specific_state_;

    std::weak_ptr<Window> owner_;
    std::wstring title_;

    zaf::InitialRectStyle initial_rect_style_{ zaf::InitialRectStyle::CenterInOwner };
    zaf::ActivateOption activate_option_{ zaf::ActivateOption::Normal };
    bool is_popup_{ false };
    bool has_border_{ true };
    bool has_title_bar_{ true };
    bool has_system_menu_{ true };
    bool is_sizable_{ true };
    bool can_maximize_{ true };
    bool can_minimize_{ true };
    bool is_tool_window_{ false };
    bool is_topmost_{ false };
    std::optional<float> min_width_;
    std::optional<float> max_width_;
    std::optional<float> min_height_;
    std::optional<float> max_height_;

    TrackMouseMode track_mouse_mode_{ TrackMouseMode::None };

    std::shared_ptr<Control> root_control_;
    std::shared_ptr<Control> mouse_over_control_;
    std::shared_ptr<Control> mouse_capture_control_;
    std::unique_ptr<internal::WindowFocusedControlManager> focused_control_manager_;
    Event<FocusedControlChangedInfo> focused_control_changed_event_;
    std::shared_ptr<TooltipWindow> tooltip_window_;

    std::weak_ptr<InspectorWindow> inspector_window_;
    std::shared_ptr<Control> highlight_control_;
    bool is_selecting_inspector_control_{};

    Event<HandleCreatedInfo> handle_created_event_;
    Event<DestroyedInfo> destroyed_event_;
    Event<MessageReceivedInfo> message_received_event_;
    Event<MessageHandledInfo> message_handled_event_;
    Event<ShowInfo> show_event_;
    Event<HideInfo> hide_event_;
    Event<ActivatedInfo> activated_event_;
    Event<DeactivatedInfo> deactivated_event_;
    Event<WindowFocusGainedInfo> focus_gained_event_;
    Event<WindowFocusLostInfo> focus_lost_event_;
    Event<ClosingInfo> closing_event_;
    Event<WindowSizeChangedInfo> size_changed_event_;
    Event<RootControlChangedInfo> root_control_changed_event_;
    Event<MouseCaptureControlChangedInfo> mouse_capture_control_changed_event_;
};

ZAF_OBJECT_BEGIN(Window)
ZAF_OBJECT_PARSER(WindowParser)
ZAF_OBJECT_PROPERTY(Owner)
ZAF_OBJECT_PROPERTY(InitialRectStyle)
ZAF_OBJECT_PROPERTY(Rect)
ZAF_OBJECT_PROPERTY(Position)
ZAF_OBJECT_PROPERTY(Size)
ZAF_OBJECT_PROPERTY(ContentRect)
ZAF_OBJECT_PROPERTY(ContentSize)
ZAF_OBJECT_PROPERTY(ContentWidth)
ZAF_OBJECT_PROPERTY(ContentHeight)
ZAF_OBJECT_PROPERTY(MinSize)
ZAF_OBJECT_PROPERTY(MaxSize)
ZAF_OBJECT_PROPERTY(Width)
ZAF_OBJECT_PROPERTY(MinWidth)
ZAF_OBJECT_PROPERTY(MaxWidth)
ZAF_OBJECT_PROPERTY(Height)
ZAF_OBJECT_PROPERTY(MinHeight)
ZAF_OBJECT_PROPERTY(MaxHeight)
ZAF_OBJECT_PROPERTY(ActivateOption)
ZAF_OBJECT_PROPERTY(IsPopup)
ZAF_OBJECT_PROPERTY(HasBorder)
ZAF_OBJECT_PROPERTY(HasTitleBar)
ZAF_OBJECT_PROPERTY(IsSizable)
ZAF_OBJECT_PROPERTY(HasSystemMenu)
ZAF_OBJECT_PROPERTY(CanMinimize)
ZAF_OBJECT_PROPERTY(CanMaximize)
ZAF_OBJECT_PROPERTY(IsToolWindow)
ZAF_OBJECT_PROPERTY(IsTopmost)
ZAF_OBJECT_PROPERTY(Title)
ZAF_OBJECT_PROPERTY(RootControl)
ZAF_OBJECT_PROPERTY(MouseCaptureControl)
ZAF_OBJECT_PROPERTY(MouseOverControl)
ZAF_OBJECT_PROPERTY(FocusedControl)
ZAF_OBJECT_PROPERTY(IsVisible)
ZAF_OBJECT_END

std::shared_ptr<Window> GetWindowFromHandle(HWND handle);

}