#pragma once

/**
@file
    Defines the `zaf::Window` class.
*/

#include <Windows.h>
#include <optional>
#include <zaf/base/event/event.h>
#include <zaf/base/non_copyable.h>
#include <zaf/base/none.h>
#include <zaf/control/control.h>
#include <zaf/graphic/rect.h>
#include <zaf/object/object.h>
#include <zaf/object/property_support.h>
#include <zaf/rx/disposable_host.h>
#include <zaf/rx/single.h>
#include <zaf/window/activate_options.h>
#include <zaf/window/event/activate_event_info.h>
#include <zaf/window/event/closing_info.h>
#include <zaf/window/event/destroy_infos.h>
#include <zaf/window/event/dpi_changed_info.h>
#include <zaf/window/event/focused_control_changed_info.h>
#include <zaf/window/event/handle_create_infos.h>
#include <zaf/window/event/message_handled_info.h>
#include <zaf/window/event/message_handling_info.h>
#include <zaf/window/event/mouse_capture_control_changed_info.h>
#include <zaf/window/event/root_control_changed_info.h>
#include <zaf/window/event/show_window_event_info.h>
#include <zaf/window/event/window_focus_event_info.h>
#include <zaf/window/event/window_size_changed_info.h>
#include <zaf/window/message/keyboard_message.h>
#include <zaf/window/message/message.h>
#include <zaf/window/screen.h>
#include <zaf/window/show_options.h>
#include <zaf/window/window_handle_state.h>
#include <zaf/window/window_messager.h>
#include <zaf/window/window_parser.h>

namespace zaf::internal {
class MessageLoop;
class WindowControlFacet;
class WindowFocusFacet;
class WindowGeometryFacet;
class WindowInspectFacet;
class WindowKeyboardFacet;
class WindowLifecycleFacet;
class WindowMouseFacet;
class WindowRenderFacet;
class WindowStyleFacet;
class WindowVisibilityFacet;
}

namespace zaf {

class HitTestMessage;
class MouseMessage;
class TooltipWindow;
class WindowClass;
class WindowClassRegistry;
class WindowHolder;
enum class HitTestResult;

/**
Represents a top-level window.
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
    Constructs the window with the default window class.

    @throw std::bad_alloc
    */
    Window();

    /**
    Constrcuts the window with the specified window class name.

    @param window_class_name
        The name of the window class.

    @throw std::bad_alloc

    @details
        This method will register a window class with the specified name if it is not registered
        yet, using the default window class properties.
    */
    explicit Window(std::wstring_view window_class_name);

    /**
    Constructs the window with the specified window class.

    @param window_class
        The window class.

    @pre
        The window class is not null.

    @throw std::bad_alloc
    @throw zaf::PreconditionError
    */
    explicit Window(std::shared_ptr<WindowClass> window_class);

    /**
    Destructs the window.
    */
    ~Window();

    /**
    Gets the window's owner.

    @return
        The owner of the window. It is null if the window has no owner.
    */
    std::shared_ptr<Window> Owner() const noexcept;

    /**
    Sets the window's owner.

    @param owner
        The owner window. It can be null to remove the owner. The owner will be stored as a weak
        pointer to avoid circular reference.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Creating`, `Created`, `Destroying` or `Destroyed`.

    @throw zaf::InvalidOperationError
        Thrown if trying to set the window itself as its owner.

    @details
        If a window has an owner, the owner's handle must be created before the window's handle is
        created, otherwise an exception will be thrown when creating the window's handle.

    @see zaf::Window::CreateHandle()
    */
    void SetOwner(std::shared_ptr<Window> owner);

    /**
    Shows the inspector window that inspects the properties and control tree of the current window.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `NotCreated`, `Creating`, `Destroying` or `Destroyed`.

    @throw ...
        Any exception thrown during the creation of the inspector window.

    @details
        The inspector window is unique for each window. Calling this method multiple times will 
        only show the existing inspector window.
    */
    void ShowInspector() const;

#pragma region Public Style Management
    /**
    @name Public Style Management
    @{
    */

    /**
    Gets the window class of the window.

    @post
        The returned window class is not null.
    */
    const std::shared_ptr<WindowClass>& Class() const noexcept;

    /**
    Gets the window's title.

    @throw std::bad_alloc
    @throw zaf::Win32Error
        Thrown if fails to get the title.

    @return
        The title of the window if the window handle state is `NotCreated`, `Creating`, `Created`
        or `Destroying`. Otherwise, returns an empty string.

    @details
        The default title is an empty string.
    */
    std::wstring Title() const;

    /**
    Sets the window's title.

    @param title
        The new title of the window.

    @throw std::bad_alloc
    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Destroying` or `Destroyed`.

    @throw zaf::Win32Error
        Thrown if fails to set the title.
    */
    void SetTitle(const std::wstring& title);

    /**
    Indicates whether the window is a popup window.

    @return
        A bool value indicates whether the window is a popup window if the window handle state is
        `NotCreated`, `Creating`, `Created` or `Destroying`. Otherwise, returns false.

    @details
        The default value is false, means that the window is an overlapped window.
    */
    bool IsPopup() const noexcept;

    /**
    Sets whether the window is a popup window.

    @param is_popup
        A bool value indicates whether the window is a popup window.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Creating`, `Created`, `Destroying` or `Destroyed`.

        Setting this property to false will also sets `HasBorder()` and `HasTitleBar()` to true, as
        an overlapped window must have border and title bar.
    */
    void SetIsPopup(bool is_popup);

    /**
    Indicates whether the window has border.

    @return
        A bool value indicates whether the window has border if the window handle state is
        `NotCreated`, `Creating`, `Created` or `Destroying`. Otherwise, returns false.

    @details
        The default value is true.
    */
    bool HasBorder() const noexcept;

    /**
    Sets whether the window has border.

    @param has_border
        A bool value indicates whether the window has border.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Creating`, `Created`, `Destroying` or `Destroyed`.

    @throw zaf::InvalidOperationError
        Thrown if trying to set this property to false when `IsPopup()` is false.

    @details
        Setting this property to false will also sets `HasTitleBar()`, `HasSystemMenu()` and
        `IsSizable()` to false.
    */
    void SetHasBorder(bool has_border);

    /**
    Indicates whether the window has title bar.

    @return
        A bool value indicates whether the window has title bar if the window handle state is
        `NotCreated`, `Creating`, `Created` or `Destroying`. Otherwise, returns false.

    @details
        The default value is true.
    */
    bool HasTitleBar() const noexcept;

    /**
    Sets whether the window has title bar.

    @param has_title_bar
        A bool value indicates whether the window has title bar.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Creating`, `Created`, `Destroying` or `Destroyed`.

    @throw zaf::InvalidOperationError
        Thrown if:
        - trying to set this property to false when `IsPopup()` is false.
        - trying to set this property to true when `HasBorder()` is false.

    @details
        Setting this property to false will also sets `HasSystemMenu()` to false.
    */
    void SetHasTitleBar(bool has_title_bar);

    /**
    Indicates whether the window has system menu.

    @return
        A bool value indicates whether the window has system menu if the window handle state is
        `NotCreated`, `Creating`, `Created` or `Destroying`. Otherwise, returns false.

    @details
        The default value is true.
    */
    bool HasSystemMenu() const noexcept;

    /**
    Sets whether the window has system menu.

    @param has_system_menu
        A bool value indicates whether the window has system menu.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Creating`, `Created`, `Destroying` or `Destroyed`.

    @throw zaf::InvalidOperationError
        Thrown if trying to set this property to true when `HasTitleBar()` is false.
    */
    void SetHasSystemMenu(bool has_system_menu);

    /**
    Indicates whether the window is sizable.

    @return
        A bool value indicates whether the window is sizable if the window handle state is
        `NotCreated`, `Creating`, `Created` or `Destroying`. Otherwise, returns false.

    @details
        The default value is true.
    */
    bool IsSizable() const noexcept;

    /**
    Sets whether the window is sizable.

    @param is_sizable
        A bool value indicates whether the window is sizable.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Destroying` or `Destroyed`.

    @throw zaf::InvalidOperationError
        Thrown if trying to set this property to true when `HasBorder()` is false.

    @throw zaf::Win32Error
        Thrown if fails to change the window style.
    */
    void SetIsSizable(bool is_sizable);

    /**
    Indicates whether the window can maximize.

    @return
        A bool value indicates whether the window can maximize if the window handle state is
        `NotCreated`, `Creating`, `Created` or `Destroying`. Otherwise, returns false.

    @details
        The default value is `true`.
     */
    bool CanMaximize() const noexcept;

    /**
    Sets whether the window can maximize.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Destroying` or `Destroyed`.

    @throw zaf::Win32Error
        Thrown if fails to change the window style.

    @details
        This property can be set to true even if `HasBorder()`, `HasTitleBar()` or
        `HasSystemMenu()` is false. However, in this case, the maximize button would not be
        displayed, but it still enables maximizing the window programmatically or when double
        clicking on an area that returns title bar on hit test.
    */
    void SetCanMaximize(bool can_maximize);

    /**
    Indicates whether the window can minimize.

    @return
        A bool value indicates whether the window can minimize if the window handle state is
        `NotCreated`, `Creating`, `Created` or `Destroying`. Otherwise, returns false.

    @details
        The default value is `true`.
    */
    bool CanMinimize() const noexcept;

    /**
    Sets whether the window can minimize.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Destroying` or `Destroyed`.

    @throw zaf::Win32Error
        Thrown if fails to change the window style.

    @details
        This property can be set to true even if `HasBorder()`, `HasTitleBar()` or
        `HasSystemMenu()` is false. However, in this case, the minimize button would not be
        displayed, but it still enables minimizing the window programmatically or when clicking the
        window on the task bar.
    */
    void SetCanMinimize(bool can_minimize);

    /**
    Indicates whether the window is a tool window.

    @return
        A bool value indicates whether the window is a tool window if the window handle state is
        `NotCreated`, `Creating`, `Created` or `Destroying`. Otherwise, returns false.

    @details
        The default value is false.
    */
    bool IsToolWindow() const noexcept;

    /**
    Sets whether the window is a tool window.

    @param is_tool_window
        A bool value indicates whether the window is a tool window.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Destroying` or `Destroyed`.

    @throw zaf::Win32Error
        Thrown if fails to change the window style.
    */
    void SetIsToolWindow(bool is_tool_window);

    /**
    Indicates whether the window is topmost.

    @return
        A bool value indicates whether the window is topmost if the window handle state is
        `NotCreated`, `Creating`, `Created` or `Destroying`. Otherwise, returns false.

    @details
        The default value is false.
    */
    bool IsTopmost() const noexcept;

    /**
    Sets whether the window is topmost.

    @param is_topmost
        A bool value indicates whether the window is topmost.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Destroying` or `Destroyed`.

    @throw zaf::Win32Error
        Thrown if fails to change the window style.
    */
    void SetIsTopmost(bool is_topmost);

    /**
    Gets the window's activate options.

    @return
        The last activate options being set, regardless of the window handle state.

    @details
        The default value is `Normal`.
    */
    ActivateOptions ActivateOptions() const noexcept;

    /**
    Sets the window's activate options.

    @param option
        The activate options to be set.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Destroying` or `Destroyed`.

    @throw zaf::Win32Error
        Thrown if fails to change the window style.
    */
    void SetActivateOptions(zaf::ActivateOptions options);

    /**
    Indicates whether the window uses a custom frame rather than the system frame.
    */
    bool UseCustomFrame() const noexcept;

    /**
    Sets whether the window uses a custom frame rather than the system frame.

    @param use_custom_frame
        A bool value indicates whether the window uses a custom frame.

    @details
        Setting this property to true will remove the system frame of the window, including the
        title bar and borders, and the window will be responsible for drawing its own frame and
        handling related behaviors, such as moving and resizing.
    */
    void SetUseCustomFrame(bool use_custom_frame) noexcept;
    /**@}*/
#pragma endregion

#pragma region Public Geometry Management
    /**
    @name Public Geometry Management
    @{
    */

    std::shared_ptr<zaf::Screen> Screen() const noexcept;
    void SetScreen(std::shared_ptr<zaf::Screen> screen);
    float DPI() const noexcept;

    /**
    Gets the window's rectangle in screen coordinate.

    @details
        The default size of a window is 640*480. The initial position of a window is centered in
        the owner if there is one, or centered in the screen, and it can be overridden by calling
        `SetRect()` or `SetPosition()` before creating the window handle.

        After creating the window handle, this method returns the actual rectangle of the window.

        If the window handle state is `Destroyed`, this method always returns an empty rectangle.
    */
    zaf::Rect Rect() const noexcept;

    /**
    Sets the window's rectangle in screen coordinate.

    @param rect
        The new rectangle to be set. It will be clamped to the minimum and maximum size of the
        window, and will be snapped to pixels according to the current screen's DPI.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Destroying` or `Destroyed`.

    @throw zaf::Win32Error
        Thrown if fails to set the rectangle.
    */
    void SetRect(const zaf::Rect& rect);

    /**
    Gets the window's position in screen coordinate.

    @details
        This is a shortcut method for `Rect().position`.
    */
    Point Position() const noexcept;

    /**
    Sets the window's position in screen coordinate.

    @param position
        The new position to be set.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Destroying` or `Destroyed`.

    @throw zaf::Win32Error
        Thrown if fails to set the position.

    @details
        This is a shortcut method for `SetRect()`.
    */
    void SetPosition(const Point& position);

    /**
    Gets the window's size.

    @details
        This is a shortcut method for `Rect().size`.
    */
    zaf::Size Size() const noexcept;

    /**
    Sets the window's size.

    @param size
        The new size to be set.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Destroying` or `Destroyed`.

    @throw zaf::Win32Error
        Thrown if fails to set the size.
    */
    void SetSize(const zaf::Size& size);

    /**
    Gets the window's width.

    @details
        This is a shortcut method for `Rect().size.width`.
    */
    float Width() const noexcept;

    /**
    Sets the window's width.

    @param width
        The new width to be set.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Destroying` or `Destroyed`.

    @throw zaf::Win32Error
        Thrown if fails to set the width.

    @details
        This is a shortcut method for `SetRect()`.
    */
    void SetWidth(float width);

    /**
    Gets the window's height.

    @details
        This is a shortcut method for `Rect().size.height`.
    */
    float Height() const noexcept;

    /**
    Sets the window's height.

    @param height
        The new height to be set.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Destroying` or `Destroyed`.

    @throw zaf::Win32Error
        Thrown if fails to set the height.

    @details
        This is a shortcut method for `SetRect()`.
    */
    void SetHeight(float height);

    /**
    Gets the window's minimum width.

    @details
        The default value is retrieved from `GetSystemMetrics(SM_CXMINTRACK)`.
    */
    float MinWidth() const noexcept;

    /**
    Sets the window's minimum width.

    @param min_width
        The new minimum width to be set.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Destroying` or `Destroyed`.

    @throw zaf::Win32Error
        Thrown if fails to change the window's width if it is less than the new minimum width.

    @details
        Setting this property may change the maximum width and the current width of the window if
        they are less than the new minimum width.
    */
    void SetMinWidth(float min_width);

    /**
    Gets the window's maximum width.

    @details
        The default value is retrieved from `GetSystemMetrics(SM_CXMAXTRACK)`.
    */
    float MaxWidth() const noexcept;

    /**
    Sets the window's maximum width.

    @param max_width
        The new maximum width to be set.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Destroying` or `Destroyed`.

    @throw zaf::Win32Error
        Thrown if fails to change the window's width if it is greater than the new maximum width.

    @details
        Setting this property may change the minimum width and the current width of the window if
        they are greater than the new maximum width.
    */
    void SetMaxWidth(float max_width);

    /**
    Gets the window's minimum height.

    @details
        The default value is retrieved from `GetSystemMetrics(SM_CYMINTRACK)`.
    */
    float MinHeight() const noexcept;

    /**
    Sets the window's minimum height.

    @param min_height
        The new minimum height to be set.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Destroying` or `Destroyed`.

    @throw zaf::Win32Error
        Thrown if fails to change the window's height if it is less than the new minimum height.

    @details
        Setting this property may change the maximum height and the current height of the window if
        they are less than the new minimum height.
    */
    void SetMinHeight(float min_height);

    /**
    Gets the window's maximum height.

    @details
        The default value is retrieved from `GetSystemMetrics(SM_CYMAXTRACK)`.
    */
    float MaxHeight() const noexcept;

    /**
    Sets the window's maximum height.

    @param max_height
        The new maximum height to be set.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Destroying` or `Destroyed`.

    @throw zaf::Win32Error
        Thrown if fails to change the window's height if it is greater than the new maximum height.

    @details
        Setting this property may change the minimum height and the current height of the window if
        they are greater than the new maximum height.
    */
    void SetMaxHeight(float max_height);

    /**
    Gets the window's minimum size.

    @details
        This is a shortcut method for `zaf::Size{ MinWidth(), MinHeight() }`.
    */
    zaf::Size MinSize() const noexcept;

    /**
    Sets the windows's minimum size.

    @param size
        The new minimum size to be set.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Destroying` or `Destroyed`.

    @throw zaf::Win32Error
        Thrown if fails to change the window's size if it is less than the new minimum size.

    @details
        This is a shortcut method for calling both `SetMinWidth()` and `SetMinHeight()`.
    */
    void SetMinSize(const zaf::Size& size);

    /**
    Gets the windows's maximum size.

    @details
        This is a shortcut method for `zaf::Size{ MaxWidth(), MaxHeight() }`.
    */
    zaf::Size MaxSize() const noexcept;

    /**
    Sets the window's maximum size.

    @param size
        The new maximum size to be set.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Destroying` or `Destroyed`.

    @throw zaf::Win32Error
        Thrown if fails to change the window's size if it is greater than the new maximum size.

    @details
        This is a shortcut method for calling both `SetMaxWidth()` and `SetMaxHeight()`.
    */
    void SetMaxSize(const zaf::Size& size);

    /**
    Gets the rectangle of the window's content area in its content coordinate.

    @return
        The content rectangle, whose position is always (0,0).

    @details
        The content area is the client area of the window, excluding the non-client area such as
        borders and title bar.

        This is a shortcut method for `zaf::Rect{ Point{0, 0}, ContentSize() }`.
    */
    zaf::Rect ContentRect() const noexcept;

    /**
    Gets the size of the window's content area.

    @return
        The size of the content area.

    @details
        If the window handle state is `NotCreated`, the content size is calculated based on the
        window size and the window styles.

        If the window handle state is `Creating`, `Created` or `Destroying`, this method returns
        the actual content size.

        If the window handle state is `Destroyed`, this method returns a zero size.
    */
    zaf::Size ContentSize() const noexcept;

    /**
    Sets the size of the window's content area.

    @param size
        The new content size to be set.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Destroying` or `Destroyed`.

    @throw zaf::Win32Error
        Thrown if fails to resize the window.

    @details
        This method adjusts the window's size to make the content area match the specified size.
    */
    void SetContentSize(const zaf::Size& size);

    /**
    Gets the width of the window's content area.

    @return
        The width of the content area.

    @details
        This is a shortcut method for `ContentSize().width`.
    */
    float ContentWidth() const noexcept;

    /**
    Sets the width of the window's content area.

    @param width
        The new content width to be set.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Destroying` or `Destroyed`.

    @throw zaf::Win32Error
        Thrown if fails to resize the window.

    @details
        This is a shortcut method for calling `SetContentSize()`.
    */
    void SetContentWidth(float width);

    /**
    Gets the height of the window's content area.

    @return
        The height of the content area.

    @details
        This is a shortcut method for `ContentSize().height`.
    */
    float ContentHeight() const noexcept;

    /**
    Sets the height of the window's content area.

    @param height
        The new content height to be set.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Destroying` or `Destroyed`.

    @throw zaf::Win32Error
        Thrown if fails to resize the window.

    @details
        This is a shortcut method for calling `SetContentSize()`.
    */
    void SetContentHeight(float height);

    /**
    Gets the event that is raised after the window's size is changed.

    @details
        @warning
            Observers of this event must not throw, otherwise the behavior is undefined.

    @see zaf::Window::OnSizeChanged()
    */
    rx::Observable<WindowSizeChangedInfo> SizeChangedEvent() const;

    /**
    Gets the event that is raised after the window's DPI is changed.

    @details
        @warning
            Observers of this event must not throw, otherwise the behavior is undefined.

    @see zaf::Window::OnDPIChanged()
    */
    rx::Observable<DPIChangedInfo> DPIChangedEvent() const;

    /**
    Indicates whether the window is in a sizing or moving state.
    */
    bool IsSizingOrMoving() const noexcept;

    /**
    Gets a single that emits when the window has exited both the sizing and moving states.

    @return
        A single that emits only `OnSuccess` and won't emit `OnError`.

    @throw std::bad_alloc

    @details
        The returned single emits immediately if:
        - The window handle state is `NotCreated` or `Destroyed`.
        - The window is not in a sizing or moving state when calling this method.

        Otherwise, it emits when the window exits both the sizing and moving states. Note that if 
        the window is destroyed before exiting these states, the single won't emit at all.
    */
    rx::Single<None> WhenNotSizingOrMoving() const;

    /**
    Transforms a position from the window's content area to its screen coordinates.

    @param position_in_window
        The position in the window's content area coordinates.

    @return
        The transformed position in the window's screen coordinates.
    */
    Point TransformToScreen(const Point& position_in_window) const noexcept;

    /**
    Transforms a position from screen coordinates to the window's content area coordinates.

    @param position_in_screen
        The position in the window's screen coordinates.

    @return
        The transformed position in the window's content area coordinates.
    */
    Point TransformFromScreen(const Point& position_in_screen) const noexcept;

    /**
    Transforms a rectangle from the window's content area to its screen coordinates.

    @param rect_in_window
        The rectangle in the window's content area coordinates.

    @return
        The transformed rectangle in the window's screen coordinates.
    */
    zaf::Rect TransformToScreen(const zaf::Rect& rect_in_window) const noexcept;

    /**
    Transforms a rectangle from screen coordinates to the window's content area coordinates.

    @param rect_in_screen
        The rectangle in the window's screen coordinates.

    @return
        The transformed rectangle in the window's content area coordinates.
    */
    zaf::Rect TransformFromScreen(const zaf::Rect& rect_in_screen) const noexcept;
    /**@}*/
#pragma endregion

#pragma region Public Lifecycle Management
    /**
    @name Public Lifecycle Management
    @{
    */

    /**
    Gets the state of the window handle.
    */
    WindowHandleState HandleState() const noexcept;

    /**
    Gets the window handle.

    @return
        A valid handle if the window handle state is `Creating`, `Created` or `Destroying`. 
        Otherwise, returns null.
    */
    HWND Handle() const noexcept;

    /**
    Creates the window handle explicitly.

    @return
        A `zaf::WindowHolder` instance that keeps the handle alive. The window handle will be
        destroyed when the holder is destructed.

    @post
        The return value is not null.

    @throw std::bad_alloc

    @throw zaf::InvalidHandleStateError
        Thrown on the following conditions:
        - Current window's handle state is `Creating`, `Destroying` or `Destroyed`.
        - If an owner is set, its handle state is `NotCreated`, `Destroying` or `Destroyed`.

    @throw zaf::Win32Error
        Thrown if fails to create the window handle.

    @throw ...
        Any exception that may be thrown during the window handle creation.

    @details
        There are following steps during the creation of the window handle:
        1. Set the window handle state to `Creating`.
        2. Create the window handle via Win32 API.
        3. Call the `OnHandleCreating()` method to raise the `HandleCreatingEvent()` when the
           WM_CREATE message is received.
        4. Set the window handle state to `Created`.
        5. Initialize resources related to the window handle, such as the renderer.
        6. Call the `OnHandleCreated()` method to raise the `HandleCreatedEvent()`.

        These steps are regarded as a single atomic operation. If any exception is thrown during
        the operation, anything that has been done will be rolled back by calling the `Destroy()`
        method. In most cases, the window handle state will be reset to `NotCreated`, so that this
        method can be called again. One exception is that if `Destroy()` method is called when the
        window handle state is `Creating`, the state will be transited to `Destroying`, and then
        `Destroyed`.

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
    Gets the event that is raised after the window handle state is transited to `Creating`.

    @details
        @warning
            Observers of this event must not throw, otherwise the behavior is undefined.

    @see zaf::Window::CreateHandle()
    @see zaf::Window::OnHandleCreating()
    */
    rx::Observable<HandleCreatingInfo> HandleCreatingEvent() const;

    /**
    Gets the event that is raised after the window handle state is transited to `Created`.

    @see zaf::Window::CreateHandle()
    @see zaf::Window::OnHandleCreated()
    */
    rx::Observable<HandleCreatedInfo> HandleCreatedEvent() const;

    /**
    Destroys the window handle.

    @details
        This method behaves differently according to the current window handle state:
        - If the state is `NotCreated`, it will be transited to `Destroyed` directly.
        - If the state is `Creating`, the creation is aborted. The state will be transited to
          `Destroying` first, and then `Destroyed`. At last, `CreateHandle()` will fail with
          `zaf::Win32Error` exception.
        - If the state is `Created`, the state will be transited to `Destroying` first, and then
          `Destroyed`.
        - If the state is `Destroying` or `Destroyed`, nothing will be done.

        After the window handle state is transited to `Destroying`, the `OnDestroying()` method
        will be called to raise the `DestroyingEvent()`. After the window handle state is transited
        to `Destroyed`, the `OnDestroyed()` method will be called to raise the `DestroyedEvent()`.
    */
    void Destroy() noexcept;

    /**
    Gets the event that is raised after the window handle state is transited to `Destroying`.

    @details
        @warning
            Observers of this event must not throw, otherwise the behavior is undefined.

    @see zaf::Window::CreateHandle()
    @see zaf::Window::Destroy();
    @see zaf::Window::OnDestroying()
    */
    rx::Observable<DestroyingInfo> DestroyingEvent() const;

    /**
    Gets the event that is raised after the window handle state is transited to `Destroyed`.

    @details
        @warning
            Observers of this event must not throw, otherwise the behavior is undefined.

    @see zaf::Window::CreateHandle()
    @see zaf::Window::Destroy();
    @see zaf::Window::OnDestroyed()
    */
    rx::Observable<DestroyedInfo> DestroyedEvent() const;

    /**
    Closes the window.

    @details
        This method behaves differently according to the current window handle state:
        - If the state is `NotCreated`, it will call the `Destroy()` method directly.
        - If the state is `Creating` or `Created`, it will send a WM_CLOSE message to the window
          handle.
        - If the state is `Destroying` or `Destroyed`, nothing will be done.
    
    @see zaf::Window::ClosingEvent()
    @see zaf::Window::Destroy()
    */
    void Close() noexcept;

    /**
    Gets the event that is raised when the window is about to be closed.

    @details
        This event is raised when the window receives a WM_CLOSE message. Observers of this event
        can call `SetCanClose(false)` on the event info to prevent the window from being closed.
        Otherwise, the `Destroy()` method will be called to destroy the window handle.

        @warning
            Observers of this event must not throw, otherwise the behavior is undefined.

    @see zaf::Window::Close()
    @see zaf::Window::Destroy()
    */
    rx::Observable<ClosingInfo> ClosingEvent() const;
    /**@}*/
#pragma endregion

#pragma region Public Visibility Management
    /**
    @name Public Visibility Management
    @{
    */

    /**
    Shows the window, creates the window handle if it has not been created.

    @param options
        The show options.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Destroying` or `Destroyed`.

    @throw ...
        Any exception thrown by the `CreateHandle()` method if fails to create the window handle.

    @details
        This method will call the `CreateHandle()` method implicitly to create the window handle.
        The returned holder of the `CreateHandle()` method is registered to the application, so 
        users don't need to keep the window or the holder by themselves. When the window handle is 
        destroyed, its holder will be unregistered from the application, so that the window 
        instance will be destructed as well.
    */
    void Show(ShowOptions options = ShowOptions::Normal);

    /**
    Shows and maximizes the window, creates the window handle if it has not been created.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Destroying` or `Destroyed`.

    @throw ...
        Any exception thrown by the `CreateHandle()` method if fails to create the window handle.

    @details
        This method is similar to the `Show()` method, except that it maximizes the window.

    @see zaf::Window::Show()
    */
    void Maximize();

    /**
    Indicates whether the window is maximized.
    */
    bool IsWindowMaximized() const noexcept;

    /**
    Shows and minimizes the window, creates the window handle if it has not been created.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Destroying` or `Destroyed`.

    @throw ...
        Any exception thrown by the `CreateHandle()` method if fails to create the window handle.

    @details
        This method is similar to the `Show()` method, except that it minimizes the window.

    @see zaf::Window::Show()
    */
    void Minimize();

    /**
    Indicates whether the window is minimized.
    */
    bool IsWindowMinimized() const noexcept;

    /**
    Shows and restores the window to its original size and position, creates the window handle if
    it has not been created.

    @throw zaf::InvalidHandleStateError
        Thrown if the window handle state is `Destroying` or `Destroyed`.

    @throw ...
        Any exception thrown by the `CreateHandle()` method if fails to create the window handle.

    @details
        This method is similar to the `Show()` method, except that it restores the window.

    @see zaf::Window::Show()
    */
    void Restore();

    /**
    Hides the window.

    @details
        This method takes effect only when the window handle state is `Creating` or `Created`, 
        otherwise it does nothing. The window is remained registered in the application after it is
        hidden.
    */
    void Hide() noexcept;

    /**
    Indicates whether the window is visible.
    */
    bool IsVisible() const noexcept;

    /**
    Gets the event that is raised when the window shows.
    */
    rx::Observable<ShowInfo> ShowEvent() const;

    /**
    Gets the event that is raised when the window hides.
    */
    rx::Observable<HideInfo> HideEvent() const;
    /**@}*/
#pragma endregion

#pragma region Public Control Management
    /**
    @name Public Control Management
    @{
    */

    /**
    Gets the window's root control.
    */
    const std::shared_ptr<Control>& RootControl() const noexcept;

    /**
    Sets the specified control as the window's root control.

    @param control
        The new root control.

    @pre
        The control is not null.

    @throw zaf::PreconditionError

    @throw zaf::InvalidOperationError
        Thrown if the control already has a parent or is already a root control of other window.

    @throw ...
        Any exception that may be thrown when setting the root control to the window.
    */
    void SetRootControl(const std::shared_ptr<Control>& control);

    rx::Observable<RootControlChangedInfo> RootControlChangedEvent() const;
    /**@}*/
#pragma endregion

#pragma region Public Focus Management
    /**
    @name Public Focus Management
    @{
    */

    /**
    Attempts to bring the window to the foreground and activates it.

    @return
        Returns true if the window is brought to foreground and activated; otherwise false.
    */
    bool BringToForeground() noexcept;

    /**
    Gets the event that is raised after the window is activated.    
    */
    rx::Observable<ActivatedInfo> ActivatedEvent() const;

    /**
    Gets the event that is raised after the window is deactivated.
    */
    rx::Observable<DeactivatedInfo> DeactivatedEvent() const;

    /**
    Indicates whether the window has input focus.
    */
    bool IsFocused() const noexcept;

    /**
    Gets the event that is raised after the window gains input focus.
    */
    rx::Observable<WindowFocusGainedInfo> FocusGainedEvent() const;

    /**
    Gets the event that is raised after the window loses input focus.
    */
    rx::Observable<WindowFocusLostInfo> FocusLostEvent() const;

    /**
    Gets the control that has input focus in the window.

    @return
        The control that has input focus in the window. Null if there is no focused control.
    */
    std::shared_ptr<Control> FocusedControl() const noexcept;

    /**
    Gets the event that is raised after the focused control in the window is changed.
    */
    rx::Observable<FocusedControlChangedInfo> FocusedControlChangedEvent() const;
    /**@}*/
#pragma endregion

#pragma region Public Rendering Management
    /**
    @name Public Rendering Management
    @{
    */

    /**
    Requests a repaint of the entire window.

    @see zaf::Window::RequestRepaint(const zaf::Rect&)
    */
    void RequestRepaint() noexcept;

    /**
    Requests a repaint of a specific area of the window.

    @param repaint_rect
        The rectangle area to be repainted, in the window's content coordinate.

    @details
        The repaint will be scheduled on the next repaint cycle. Multiple calls to this method 
        before the next repaint cycle may be coalesced into a single repaint.
    */
    void RequestRepaint(const zaf::Rect& repaint_rect) noexcept;

    /**
    Repaints the window immediately if there is any pending repaint request.
    */
    void RepaintIfNeeded() noexcept;

    /**@}*/
#pragma endregion

#pragma region Public Mouse Input Handling
    /**
    @name Public Mouse Input Handling
    @{
    */

    /**
    Gets the control which is under the mouse cursor.
    */
    std::shared_ptr<Control> MouseOverControl() const noexcept;

    /**
    Gets the control which captures mouse in the window.
    */
    std::shared_ptr<Control> MouseCaptureControl() const noexcept;

    /**
    Gets the event that is raised after the mouse capture control is changed.
    */
    rx::Observable<MouseCaptureControlChangedInfo> MouseCaptureControlChangedEvent() const;

    /**
    Gets the position of the mouse cursor in the window's content coordinate.
    */
    Point MousePosition() const noexcept;

    /**@}*/
#pragma endregion

#pragma region Public Message Handling
    /**
    @name Public Message Handling
    @{
    */

    /**
    Gets the event that is raised when the window receives a message and is about to handle it.
    */
    rx::Observable<MessageHandlingInfo> MessageHandlingEvent() const;

    /**
    Gets the event that is raised after a message is handled by the window.
    */
    rx::Observable<MessageHandledInfo> MessageHandledEvent() const;

    /**
    Gets the messager to send or post messages to the window.

    @details
        This method wouldn't throw even if the window handle state is `NotCreated` or `Destroyed`.
        The exception is delayed until sending or posting a message.
    */
    WindowMessager Messager() noexcept;
    /**@}*/
#pragma endregion

protected:
    void Initialize() override;

#pragma region Protected Geometry Management
    /**
    @name Protected Geometry Management
    @{
    */

    /**
    Called after the window's size is changed.

    @param event_info
        Information of the event.

    @details
        This method is called when the window receives WM_SIZE message, after the internal resize
        handling is done.

        The default implementation of this method raises the `SizeChangedEvent()`. Derived classes
        should call the same method of base class if they override this method.

        @warning
            This method must not throw, otherwise the behavior is undefined.
    */
    virtual void OnSizeChanged(const WindowSizeChangedInfo& event_info);

    /**
    Called after the window's DPI is changed.

    @param event_info
        Information of the event.

    @details
        This method is called when the window receives WM_DPICHANGED message, after changing the
        window to the suggested rectangle.

        The default implementation of this method raises the `DPIChangedEvent()`. Derived classes
        should call the same method of base class if they override this method.

        @warning
            This method must not throw, otherwise the behavior is undefined.
    */
    virtual void OnDPIChanged(const DPIChangedInfo& event_info);
    /**@}*/
#pragma endregion

#pragma region Protected Lifecycle Management
    /**
    @name Protected Lifecycle Management
    @{
    */

    /**
    Called after the window handle state is transited to `Creating`.

    @param event_info
        Information of the event.

    @details
        This method is called right after the window receives `WM_CREATE` message, before any
        internal initialization related to the window handle is done.

        The default implementation of this method raises the `HandleCreatingEvent()`. Derived
        classes should call the same method of the base class if they override this method.

        @warning
            This method must not throw, otherwise the behavior is undefined.

    @see zaf::Window::CreateHandle()
    */
    virtual void OnHandleCreating(const HandleCreatingInfo& event_info);

    /**
    Called after the window handle state is transited to `Created`.

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
    Called after the window handle state is transited to `Destroying`.

    @param event_info
        Information of the event.

    @details
        This method is called right after the window receives `WM_DESTROY` message, before any
        internal cleanup related to the window handle is done. During the execution of this method,
        the window handle is still valid.

        The default implementation of this method raises the `DestroyingEvent()`. Derived classes
        should call the same method of the base class if they override this method.

        @warning
            This method must not throw, otherwise the behavior is undefined.
    */
    virtual void OnDestroying(const DestroyingInfo& event_info);

    /**
    Called after the window handle state is transited to `Destroyed`.

    @param event_info
        Information of the event.

    @details
        This method is called after the internal cleanup related to the window handle is done. The
        window handle has been detached from the window, and is about to be destroyed. It is still
        valid and can be retrieved via the `WindowHandle()` method of the event info.

        @note
            `WindowHandle()` may return null if `Destroy()` method is called when the window handle
            state is `NotCreated`.

        The default implementation of this method raises the `DestroyedEvent()`. Derived classes 
        should call the same method of the base class if they override this method.

        @warning
            This method must not throw, otherwise the behavior is undefined.
    */
    virtual void OnDestroyed(const DestroyedInfo& event_info);

    /**
    Called when the window is about to be closed.

    @param event_info
        Information of the event.

    @details
        This method is called when the window receives a WM_CLOSE message. The default 
        implementation of this method raises the `ClosingEvent()`. Derived classes should call the 
        same method of the base class if they override this method.

        @warning
            This method must not throw, otherwise the behavior is undefined.
    */
    virtual void OnClosing(const ClosingInfo& event_info);
    /**@}*/
#pragma endregion

#pragma region Protected Visibility Management
    /**
    @name Protected Visibility Management
    @{
    */

    /**
    Called when the window shows.

    @param event_info
        Information of the event.
        
    @details
        The default implementation of this method raises the `ShowEvent()`. Derived classes should 
        call the same method of base class if they override this method.
    */
    virtual void OnShow(const ShowInfo& event_info);

    /**
    Called when the window hides.

    @param event_info
        Information of the event.

    @details
        The default implementation of this method raises the `HideEvent()`. Derived classes should 
        call the same method of base class if they override this method.
    */
    virtual void OnHide(const HideInfo& event_info);
    /**@}*/
#pragma endregion

#pragma region Protected Control Management
    /**
    @name Protected Control Management
    @{
    */
    virtual void OnRootControlChanged(const RootControlChangedInfo& event_info);
    /**@}*/
#pragma endregion

#pragma region Protected Focus Management
    /**
    @name Protected Focus Management
    @{
    */

    /**
    Called after the window is activated.

    @param event_info
        Information of the event.

    @details
        This method is called when the window receives WM_ACTIVATE message, whose wParam is not
        WA_INACTIVE. The default implementation of this method raises the `ActivatedEvent()`. 
        Derived class should call the same method of base class if they override this method.

        @warning
            This method must not throw, otherwise the behavior is undefined.
    */
    virtual void OnActivated(const ActivatedInfo& event_info);

    /**
    Call after the window is deactivated.

    @param event_info
        Information of the event.

    @details
        This method is called when the window receives WM_ACTIVATE message, whose wParam is
        WA_INACTIVE. The default implementation of this method raises the `DeactivatedEvent()`. 
        Derived classes should call the same method of base class if they override this method.

        @warning
            This method must not throw, otherwise the behavior is undefined.
    */
    virtual void OnDeactivated(const DeactivatedInfo& event_info);

    /**
    Called after the window gains input focus.

    @param event_info
        Information of the event.

    @details
        This method is called when the window receives WM_SETFOCUS message, after the internal
        focus handling is done. The default implementation of this method raises the
        `FocusGainedEvent()`. Derived classes should call the same method of base class if they
        override this method.

        @warning
            This method must not throw, otherwise the behavior is undefined.
    */
    virtual void OnFocusGained(const WindowFocusGainedInfo& event_info);

    /**
    Called after the window loses input focus.

    @param event_info
        Information of the event.

    @details
        This method is called when the window receives WM_KILLFOCUS message, after the internal
        focus handling is done. The default implementation of this method raises the
        `FocusLostEvent()`. Derived classes should call the same method of base class if they
        override this method.

        @warning
            This method must not throw, otherwise the behavior is undefined.
    */
    virtual void OnFocusLost(const WindowFocusLostInfo& event_info);

    /**
    Called after the focused control in the window is changed.
    
    @param event_info
        Information of the event.

    @details
        The default implementation of this method raises the `FocusedControlChangedEvent()`. 
        Derived classes should call the same method of base class if they override this method.
    */
    virtual void OnFocusedControlChanged(const FocusedControlChangedInfo& event_info);
    /**@}*/
#pragma endregion

#pragma region Protected Mouse Input Handling
    /**
    @name Protected Mouse Input Handling
    @{
    */

    virtual std::optional<HitTestResult> HitTest(const HitTestMessage& message);

    /**
    Called after the mouse capture control is changed.

    @param event_info
        Information of the event.
        
    @details
        The default implementation of this method raises the `MouseCaptureChangedEvent()`. Derived 
        classes should call the same method of base class if they override this method.
    */
    virtual void OnMouseCaptureControlChanged(const MouseCaptureControlChangedInfo& event_info);
    /**@}*/
#pragma endregion

#pragma region Protected Keyboard Input Handling
    /**
    @name Protected Keyboard Input Handling
    @{
    */

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

    /**@}*/
#pragma endregion

#pragma region Protected Message Handling
    /**
    @name Protected Message Handling
    @{
    */

    /**
    Called when the window receives a message and is about to handle it.

    @param event_info
        Information of the event. Call `MarkAsHandled()` to prevent the message from being handled 
        by default.

    @details
        The default implementation of this method raises the `MessageHandlingEvent()`. Derived 
        classes should call the same method of base class if they override this method.
        
        @warning
            This method must not throw, otherwise the behavior is undefined.
    */
    virtual void OnMessageHandling(const MessageHandlingInfo& event_info);

    /**
    Called after the message is handled, including the default window procedure.

    @param event_info
        Information of the event.

    @details
        The default implementation of this method will raise MessageHandledEvent. Derived classes
        should call the same method of base class if they override this method.

        @warning
            This method must not throw, otherwise the behavior is undefined.
    */
    virtual void OnMessageHandled(const MessageHandledInfo& event_info);
    /**@}*/
#pragma endregion

private:
    static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
    LRESULT RouteWindowMessage(HWND hwnd, UINT id, WPARAM wparam, LPARAM lparam);
    static bool CanMessageBeInterupted(const UINT message_id) noexcept;
    std::optional<LRESULT> HandleMessage(const Message& message);
    void HandleWMSHOWWINDOW(const ShowWindowMessage& message);
    
private:
    std::weak_ptr<Window> owner_;

    // Style Related
    std::unique_ptr<internal::WindowStyleFacet> style_facet_;

    // Geometry Related
    std::unique_ptr<internal::WindowGeometryFacet> geometry_facet_;
    Event<WindowSizeChangedInfo> size_changed_event_;
    Event<DPIChangedInfo> dpi_changed_event_;

    // Lifecycle Related
    std::unique_ptr<internal::WindowLifecycleFacet> lifecycle_facet_;
    Event<HandleCreatingInfo> handle_creating_event_;
    Event<HandleCreatedInfo> handle_created_event_;
    Event<ClosingInfo> closing_event_;
    Event<DestroyingInfo> destroying_event_;
    Event<DestroyedInfo> destroyed_event_;

    // Visibility Related
    std::unique_ptr<internal::WindowVisibilityFacet> visibility_facet_;
    Event<ShowInfo> show_event_;
    Event<HideInfo> hide_event_;

    // Control Related
    std::unique_ptr<internal::WindowControlFacet> control_facet_;
    Event<RootControlChangedInfo> root_control_changed_event_;

    // Rendering Related
    std::unique_ptr<internal::WindowRenderFacet> render_facet_;

    // Focus Related
    std::unique_ptr<internal::WindowFocusFacet> focus_facet_;
    Event<ActivatedInfo> activated_event_;
    Event<DeactivatedInfo> deactivated_event_;
    Event<WindowFocusGainedInfo> focus_gained_event_;
    Event<WindowFocusLostInfo> focus_lost_event_;
    Event<FocusedControlChangedInfo> focused_control_changed_event_;

    // Mouse input Related
    std::unique_ptr<internal::WindowMouseFacet> mouse_facet_;
    Event<MouseCaptureControlChangedInfo> mouse_capture_control_changed_event_;

    // Keyboard input Related
    std::unique_ptr<internal::WindowKeyboardFacet> keyboard_facet_;
    
    // Message Handling Related
    Event<MessageHandlingInfo> message_handling_event_;
    Event<MessageHandledInfo> message_handled_event_;

    // Inspection Related
    std::unique_ptr<internal::WindowInspectFacet> inspect_facet_;

    friend class Application;
    friend class Control;
    friend class InspectorWindow;
    friend class WindowClassRegistry;
    friend class internal::MessageLoop;
    friend class internal::WindowControlFacet;
    friend class internal::WindowFocusFacet;
    friend class internal::WindowGeometryFacet;
    friend class internal::WindowInspectFacet;
    friend class internal::WindowKeyboardFacet;
    friend class internal::WindowLifecycleFacet;
    friend class internal::WindowMouseFacet;
    friend class internal::WindowRenderFacet;
    friend class internal::WindowStyleFacet;
    friend class internal::WindowVisibilityFacet;
};

ZAF_OBJECT_BEGIN(Window)
ZAF_OBJECT_PARSER(WindowParser)
ZAF_OBJECT_PROPERTY(Owner)
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
ZAF_OBJECT_PROPERTY(ActivateOptions)
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
ZAF_OBJECT_PROPERTY(UseCustomFrame)
ZAF_OBJECT_END

std::shared_ptr<Window> GetWindowFromHandle(HWND handle);

}