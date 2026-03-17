#pragma once

#include <Windows.h>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <zaf/base/event/event.h>
#include <zaf/base/non_copyable.h>
#include <zaf/control/anchor.h>
#include <zaf/control/event/double_click_info.h>
#include <zaf/control/event/focus_event_info.h>
#include <zaf/control/event/is_enabled_changed_info.h>
#include <zaf/control/event/is_selected_changed_info.h>
#include <zaf/control/event/is_visible_changed_info.h>
#include <zaf/control/event/keyboard_event_info.h>
#include <zaf/control/event/mouse_capture_event_info.h>
#include <zaf/control/event/mouse_cursor_changing_info.h>
#include <zaf/control/event/mouse_event_info.h>
#include <zaf/control/event/mouse_hover_info.h>
#include <zaf/control/event/mouse_over_event_info.h>
#include <zaf/control/event/parent_changed_info.h>
#include <zaf/control/event/rect_changed_info.h>
#include <zaf/control/event/style_update_info.h>
#include <zaf/control/event/window_changed_info.h>
#include <zaf/control/image_layout.h>
#include <zaf/control/image_picker.h>
#include <zaf/control/layout/layouter.h>
#include <zaf/control/control_update_guard.h>
#include <zaf/control/style/color_picker.h>
#include <zaf/graphic/d2d/bitmap_renderer.h>
#include <zaf/graphic/color.h>
#include <zaf/graphic/frame.h>
#include <zaf/graphic/rect.h>
#include <zaf/internal/control/color_field.h>
#include <zaf/object/object.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/disposable_host.h>

namespace zaf::internal {
class ControlEventInvokerBinder;
class ControlGeometryFacet;
class ControlUpdateLock;
class ControlUpdateState;
class InspectorPort;
class WindowControlFacet;
class WindowFocusedControlManager;
class WindowKeyboardFacet;
class WindowLifecycleFacet;
class WindowMouseFacet;
class WindowRenderFacet;
}

namespace zaf {

class Canvas;
class IMECompositionInfo;
class IMEEndCompositionInfo;
class IMEStartCompositionInfo;
class Message;
class MouseMessage;
class MouseWheelMessage;
class HitTestMessage;
class Window;
enum class HitTestResult;

/**
 Represents a control in a window.

 This is the base class of all controls.
 */
class Control : 
    public Object, 
    public rx::DisposableHost,
    public std::enable_shared_from_this<Control>,
    NonCopyableNonMovable {

public:
    ZAF_OBJECT;

public:
    Control();
    virtual ~Control();

    [[nodiscard]]
    ControlUpdateGuard BeginUpdate();

#pragma region Public Geometry Management
    /**
    @name Public Geometry Management
    @{
    */

    /**
    Gets the control's rectangle in its container's coordinate.

    @return
        The rectangle of the control in its container's coordinate.

    @details
        If the control is the root control of a window, the rectangle is expressed in the
        coordinate space of the window's content area; otherwise the rectangle is expressed in the
        coordinate space of its parent's content area.

        The default rectangle of a control is an empty rectangle at position (0, 0).
    */
    const zaf::Rect& Rect() const noexcept;

    /**
    Sets the control's rectangle in its container's coordinate.

    @param rect
        The new rect to be set.

    @throw ...
        Any exception may be thrown while:
        - Calculating the preferred size for auto size, if it is enabled.
        - Updating the layout of children or parent.
        - Raising position, size or rect changed events.

    @see zaf::Control::Rect()
    */
    void SetRect(const zaf::Rect& rect);

    /**
    Gets the control's rectangle in its own coordinate.

    @return
        The rectangle of the control in its own coordinate, whose position is always `(0, 0)`.
    */
    zaf::Rect RectInSelf() const noexcept;

    /**
    Gets the control's rectangle in its window's coordinate.

    @return
        The rectangle of the control in its window's coordinate. If the control is not in a
        window, `std::nullopt` is returned.
    */
    std::optional<zaf::Rect> RectInWindow() const noexcept;

    /**
    Gets the control's position in its container's coordinate.

    @return
        The position of the control in its container's coordinate.

    @details
        This is a shortcut method for `Rect().position`.

    @see zaf::Control::Rect()
    */
    const Point& Position() const noexcept;

    /**
    Sets the control's position in its container's coordinate.

    @param position
        The new position to be set.

    @throw ...
        Any exception may be thrown while setting the new position, see `SetRect()` for details.

    @details
        This a shortcut method for calling `SetRect()` with a new position while keeping the size.
    */
    void SetPosition(const Point& position);

    /**
    Gets the control's position in its window's coordinate.

    @return
        The position of the control in its window's coordinate space. If the control is not in a
        window, std::nullopt is returned.
    */
    std::optional<zaf::Point> PositionInWindow() const noexcept;

    /**
    Gets the control's X coordinate in its container's coordinate.

    @return
        The X coordinate of the control in its container's coordinate.

    @details
        This is a shortcut method for `Position().x`.
    */
    float X() const noexcept;

    /**
    Sets the control's X coordinate in its container's coordinate.

    @param x
        The new X coordinate to be set.

    @throw ...
        Any exception may be thrown while setting the new X, see `SetRect()` for details.

    @details
        This is a shortcut method for calling `SetPosition()` with a new X coordinate while
        keeping the Y coordinate.
    */
    void SetX(float x);

    /**
    Gets the control's Y coordinate in its container's coordinate.

    @return
        The Y coordinate of the control in its container's coordinate.

    @details
        This is a shortcut method for `Position().y`.
    */
    float Y() const noexcept;

    /**
    Sets the control's Y coordinate in its container's coordinate.

    @param y
        The new Y coordinate to be set.

    @throw ...
        Any exception may be thrown while setting the new Y, see `SetRect()` for details.

    @details
        This is a shortcut method for calling `SetPosition()` with a new Y coordinate while
        keeping the X coordinate.
    */
    void SetY(float y);

    /**
    Gets the control's size.

    @return
        The size of the control.

    @details
        This is a shortcut method for `Rect().size`.
    */
    const zaf::Size& Size() const noexcept;

    /**
    Sets the control's size;

    @param size
        The new size to be set.

    @throw ...
        Any exception may be thrown while setting the new size, see `SetRect()` for details.

    @details
        This is a shortcut method for calling `SetRect()` with a new size while keeping the 
        position.
    */
    void SetSize(const zaf::Size& size);

    /**
    Gets the control's width.

    @return
        The width of the control.

    @details
        This is a shortcut method for `Size().width`.
    */
    float Width() const noexcept;

    /**
    Sets the control's width.

    @param width
        The new width to be set.

    @throw ...
        Any exception may be thrown while setting the new width, see `SetRect()` for details.

    @details
        This is a shortcut method for calling `SetSize()` with a new width while keeping the 
        height.
    */
    void SetWidth(float width);

    /**
    Gets the control's height.

    @return
        The height of the control.

    @details
        This is a shortcut method for `Size().height`.
    */
    float Height() const noexcept;

    /**
    Sets the control's height.

    @param height
        The new height to be set.

    @throw ...
        Any exception may be thrown while setting the new height, see `SetRect()` for details.

    @details
        This is a shortcut method for calling `SetSize()` with a new height while keeping the 
        width.
    */
    void SetHeight(float height);

    /**
    Gets the control's minimum width.

    @return
        The minimum width of the control.

    @details
        The default minimum width is 0.
    */
    float MinWidth() const noexcept;

    /**
    Sets the control's minimum width.

    @param min_width
        The new minimum width to be set.

    @throw ...
        Any exception may be thrown while changing the width, see `SetRect()` for details.

    @details
        Setting this property may change the maximum width and the current width of the control if
        they are less than the new minimum width.
    */
    void SetMinWidth(float min_width);

    /**
    Gets the control's maximum width.

    @return
        The maximum width of the control.

    @details
        The default maximum width is the maximum value of float.
    */
    float MaxWidth() const noexcept;

    /**
    Sets the control's maximum width.

    @param max_width
        The new maximum width to be set.

    @throw ...
        Any exception may be thrown while changing the width, see `SetRect()` for details.

    @details
        Setting this property may change the minimum width and the current width of the control if
        they are greater than the new maximum width.
    */
    void SetMaxWidth(float max_width);

    /**
    Sets the control's fixed width.

    @param width
        The fixed width to be set.

    @throw ...
        Any exception may be thrown while changing the width, see `SetRect()` for details.

    @details
        This is a shortcut method for setting both minimum width and maximum width to the same 
        value.
    */
    void SetFixedWidth(float width);

    /**
    Gets the control's minimum height.

    @return
        The minimum height of the control.

    @details
        The default minimum height is 0.
     */
    float MinHeight() const noexcept;

    /**
    Sets the control's minimum height.

    @param min_height
        The new minimum height to be set.

    @throw ...
        Any exception may be thrown while changing the height, see `SetRect()` for details.

    @details
        Setting this property may change the maximum height and the current height of the control
        if they are less than the new minimum height.
    */
    void SetMinHeight(float min_height);

    /**
    Gets the control's maximum height.

    @return
        The maximum height of the control.

    @details
        The default maximum height is the maximum value of float.
    */
    float MaxHeight() const noexcept;

    /**
    Sets the control's maximum height.

    @param max_height
        The new maximum height to be set.

    @throw ...
        Any exception may be thrown while changing the height, see `SetRect()` for details.

    @details
        Setting this property may change the minimum height and the current height of the control 
        if they are greater than the new maximum height.
    */
    void SetMaxHeight(float max_height);

    /**
    Sets the control's fixed height.

    @param height
        The fixed height to be set.

    @throw ...
        Any exception may be thrown while changing the height, see `SetRect()` for details.

    @details
        This is a shortcut method for setting both minimum height and maximum height to the same
        value.
    */
    void SetFixedHeight(float height);

    /**
    Sets the control's fixed size.

    @param size
        The fixed size to be set.

    @throw ...
        Any exception may be thrown while changing the size, see `SetRect()` for details.

    @details
        This is a shortcut method for setting both minimum size and maximum size to the same value.
    */
    void SetFixedSize(const zaf::Size& size);

    zaf::Size CalculatePreferredSize() const;
    zaf::Size CalculatePreferredSize(const zaf::Size& bound_size) const;

    bool AutoWidth() const;
    void SetAutoWidth(bool value);

    bool AutoHeight() const;
    void SetAutoHeight(bool value);

    void SetAutoSize(bool value);

    float ClampWidth(float width) const noexcept;
    float ClampHeight(float height) const noexcept;
    zaf::Size ClampSize(const zaf::Size& size) const noexcept;

    /**
     Get the control's anchor.

     The default value is Anchor::None.
     */
    Anchor Anchor() const;

    /**
     Set the control's anchor.
     */
    void SetAnchor(zaf::Anchor anchor);

    const Frame& Margin() const;

    void SetMargin(const Frame& margin);

    const Frame& Border() const;
    void SetBorder(const Frame& border);

    const Frame& Padding() const;
    void SetPadding(const Frame& padding);

    /**
    Gets the rectangle of the control's content area in the window's coordinate space.

    @return
        The content rectangle in the window's coordinate space. If the control is not in a window, 
        std::nullopt is returned.
    */
    std::optional<zaf::Rect> ContentRectInWindow() const noexcept;

    /**
    Gets the rectangle of the control's content area in the control's own coordinate space.

    @return
        The content rectangle in the control's coordinate space.
    */
    zaf::Rect ContentRectInSelf() const noexcept;

    /**
    Gets the rectangle of the control's content area in the content coordinate space.

    @return
        The content rectangle, whose position is always (0,0).
    
    @details
        The content area is the area inside the control, excluding the border and padding. Child
        controls are placed within the content area.
    */
    zaf::Rect ContentRect() const noexcept;

    /**
     Get the control's content size.
     */
    zaf::Size ContentSize() const;

    /**
    Gets the event that is raised when the position of the control is changed.
    */
    rx::Observable<PositionChangedInfo> PositionChangedEvent() const;

    /**
    Gets the event that is raised when the size of the control is changed.
    */
    rx::Observable<SizeChangedInfo> SizeChangedEvent() const;

    /**
    Gets the event that is raised when the rectangle of the control is changed.

    @details
        This event is raised after `PositionChangedEvent` and `SizeChangedEvent`.
    */
    rx::Observable<RectChangedInfo> RectChangedEvent() const;

    /** @} */
#pragma endregion

    std::shared_ptr<Image> BackgroundImage() const;

    ImagePicker BackgroundImagePicker() const;

    void SetBackgroundImage(const std::shared_ptr<Image>& image);

    void SetBackgroundImagePicker(const ImagePicker& image_picker);

    ImageLayout BackgroundImageLayout() const;
    void SetBackgroundImageLayout(ImageLayout image_layout);

    /**
     Get background color.
     */
    Color BackgroundColor() const;

    /**
     Set background color.
     */
    void SetBackgroundColor(const Color& color);

    const ColorPicker& BackgroundColorPicker() const;
    void SetBackgroundColorPicker(ColorPicker picker);

    /**
     Get border color.
     */
    Color BorderColor() const;

    /**
     Set border color.
     */
    void SetBorderColor(const Color& color);

    const ColorPicker& BorderColorPicker() const;
    void SetBorderColorPicker(ColorPicker picker);

    /**
     Get the layouter.
     */
    std::shared_ptr<Layouter> Layouter() const;

    /**
     Set the layouter.

     Setting the layouter would causes the control to relayout immediately.
     */
    void SetLayouter(const std::shared_ptr<zaf::Layouter>& layouter);

    bool HasChildren() const;
    std::size_t ChildCount() const;

    const std::shared_ptr<Control>& GetChildAtIndex(std::size_t index) const;

    /**
     Get the control's children.
     */
    const std::vector<std::shared_ptr<Control>>& Children() const;

    /**
    Removes all existing children and adds the specified children to the control.

    @param children
        The new children to be added.
    */
    void SetChildren(const std::vector<std::shared_ptr<Control>>& children);

    /**
     Add specified child to the control.
     */
    void AddChild(const std::shared_ptr<Control>& child);

    void AddChildAtIndex(const std::shared_ptr<Control>& child, std::size_t index);

    /**
     Add specified children to the control.
     */
    void AddChildren(const std::vector<std::shared_ptr<Control>>& children);

    void AddChildrenAtIndex(const std::vector<std::shared_ptr<Control>>& children);

    /**
    Removes a child at the specified index.

    @param index
        Index of the child being removed.

    @throw std::logic_error
        Thrown if index is out of range.
    */
    void RemoveChildAtIndex(std::size_t index);

    /**
     Removes specified child from the control.
     */
    void RemoveChild(const std::shared_ptr<Control>& child);

    /**
     Remove all children from the control.
     */
    void RemoveAllChildren();

    std::shared_ptr<Control> FindChild(const std::wstring& name) const;

    template<typename T>
    std::shared_ptr<T> FindChild(const std::wstring& name) const {
        return std::dynamic_pointer_cast<T>(FindChild(name));
    }

    template<typename T>
    bool BindChild(const std::wstring& name, std::shared_ptr<T>& child) const {
        child = FindChild<T>(name);
        return !!child;
    }

    std::shared_ptr<Control> FindChildAtPosition(const Point& position) const;

    std::shared_ptr<Control> FindChildAtPositionRecursively(const Point& position) const;

    /**
    Determines whether the current control is the direct parent of the specified control.
    */
    bool IsParentOf(const Control& control) const;

    /**
    Determines whether the current control is the direct or indirect parent of the specified 
    control.
    */
    bool IsAncestorOf(const Control& control) const;

    /**
    Determines whether the current control is either the same as the specified control, or is the
    direct or indirect parent of the specified control.
    */
    bool IsSameOrAncestorOf(const Control& control) const;

    /**
     Get the control's parent.

     Return nullptr if the control does not have parent.
     */
    std::shared_ptr<Control> Parent() const;

    /**
     Get the control's name.

     The name is used to identify a child in control. Multiple 
     children may have the same name. The default name is empty.
     */
    std::wstring Name() const;

    /**
     Set the control's name.

     See also Name.
     */
    void SetName(const std::wstring& name);

    /**
     Get the window where the control locates.

     Return nullptr if the control does not locates in any window.
     */
    std::shared_ptr<Window> Window() const;

    /**
    Determines whether the control is visible within the context of the parent.
    */
    bool IsVisibleInContext() const;

    /**
    Determines whether the control itself is visible. This method doesn't consider the visibility
    within the context of the parent.

    The default value is true.
    */
    bool IsVisible() const;

    /**
    Sets the visibility of the control.

    Changing the visibility will raise IsVisibleChangedEvent.
    */
    void SetIsVisible(bool is_visible);

    rx::Observable<IsVisibleChangedInfo> IsVisibleChangedEvent() const;

    /**
    Determines whether the control is enabled within the context of the parent.
    */
    bool IsEnabledInContext() const;

    /**
    Determines whether the control itself is enabled. This method doesn't consider whether it is
    enabled within the context of the parent.

    The default value is true.
    */
    bool IsEnabled() const;

    /**
    Sets whether the control is enabled.

    Changing the enabled state will raise IsEnabledChangedEvent.
    */
    void SetIsEnabled(bool is_enabled);

    rx::Observable<IsEnabledChangedInfo> IsEnabledChangedEvent() const;

    /**
    Determines whether the control is selected within the context of the parent.
    */
    bool IsSelectedInContext() const;

    /**
    Determines whether the control itself is selected. This method doesn't consider whether it is 
    selected within the context of the parent.

    The default value is false.
    */
    bool IsSelected() const;

    /**
    Sets whether the control is selected.

    Changing the selection state will raise IsSelectedChangedEvent.
    */
    void SetIsSelected(bool is_selected);

    rx::Observable<IsSelectedChangedInfo> IsSelectedChangedEvent() const;

    /**
     Get a value indicating that whether the control itself is under mouse cursor.
     */
    bool IsMouseOver() const;

    /**
    Indicates whether the control or any of its children is under mouse cursor.
    */
    bool ContainsMouse() const;

    /**
     Get a value indicating that whether the control can be focused.

     The default value is false.
     */
    bool CanFocus() const;

    /**
     Set a value indicating that whether the control can be focused.

     See also CanFocus.
     */
    void SetCanFocus(bool can_focused);

    /**
     Get a value indicating that whether the control and its children can be focused 
     using tab key.

     The default value is true.
     */
    bool CanTabStop() const;

    /**
     Get a value indicating that whether the control and its children can be focused
     using tab key.
     */
    void SetCanTabStop(bool can_tab_stop);

    /**
     Get the control's tab index within its parent.

     The default value is std::nullopt.
     */
    std::optional<std::size_t> TabIndex() const;

    /**
     Set the control's tab index within its parent.
     */
    void SetTabIndex(std::size_t tab_index);

    /**
     Get a value indicating that whether the control is focused.
     */
    bool IsFocused() const;

    /**
     Set a value indicating that whether the control is focused.

     This methods takes effect only when the control is contained in a window.
     */
    void SetIsFocused(bool is_focused);

    /**
    Determines whether the control itself or its parent is focused.
    */
    bool IsInFocusedContext() const;

    bool ContainsFocus() const;

    bool IsCachedPaintingEnabled() const;

    void SetIsCachedPaintingEnabled(bool value);

    bool CanDoubleClick() const;
    void SetCanDoubleClick(bool can_double_click);

    std::wstring Tooltip() const;
    void SetTooltip(const std::wstring& tooltip);

    /**
     Capture the mouse.
     */
    void CaptureMouse();

    /**
    Indicates whether the control is capturing the mouse.
    */
    bool IsCapturingMouse() const noexcept;

    /**
    Gets mouse captured event. This event is raised after the control calling CaptureMouse() and 
    capturing mouse successfully.
    */
    rx::Observable<MouseCapturedInfo> MouseCapturedEvent() const;

    /**
     Release the mouse.
     */
    void ReleaseMouse();

    /**
    Gets mouse released event. This event is raised after the control releasing mouse capture with 
    ReleaseMouse(), or losing mouse capture passively.
    */
    rx::Observable<MouseReleasedInfo> MouseReleasedEvent() const;

    /**
     The the mouse position in control's coordinate.
     */
    const Point GetMousePosition() const;

    float GetDPI() const;

    /**
     Determinate whether the control accepts specified key message.

     @param message
         Information of the message.
     
     @return
         Return true if the control accepts the message, otherwise return false.

     If this method returns true, preprocessing of the message is forbidden, so 
     that the message can be dispatched normally. For instance, TAB key down message
     is usually preprocessed by window to switch the focused control, therefore
     controls would not receive this event. However, a control can return true 
     against the TAB key down message in this method to prevent the preprocessing,
     then it can receive the event.
     */
    virtual bool AcceptKeyMessage(const KeyMessage& message);

    void NeedUpdateStyle();

    /**
     Requires the control to repaint.
     */
    void NeedRepaint();

    /**
     Requires the control to repaint specified rectangle area.
     */
    void NeedRepaintRect(const zaf::Rect& rect);

    /**
    Translates a position from the coordinate space of the current control to the coordinate space
    of its parent control.

    @param position
        The position in the coordinate space of the current control.

    @return 
        The translated position in the coordinate space of the parent control.

    @throw std::logic_error
        Thrown if there is no parent for the current control.
    */
    Point TranslateToParent(const Point& position) const;

    /**
    Translates a position from the coordinate space of the parent to the coordinate space of the
    current control.

    @param position
        The position in the coordinate space of the current control's parent.

    @return
        The translated position in the coordinate space of the current control. 

    @throw std::logic_error
        Thrown if there is no parent for the current control.
    */
    Point TranslateFromParent(const Point& position) const;

    rx::Observable<StyleUpdateInfo> StyleUpdateEvent() const;

    rx::Observable<PreFocusGainedInfo> PreFocusGainedEvent() const;
    rx::Observable<FocusGainedInfo> FocusGainedEvent() const;

    rx::Observable<PreFocusLostInfo> PreFocusLostEvent() const;
    rx::Observable<FocusLostInfo> FocusLostEvent() const;

    rx::Observable<MouseCursorChangingInfo> MouseCursorChangingEvent() const;

    rx::Observable<PreMouseMoveInfo> PreMouseMoveEvent() const;
    rx::Observable<MouseMoveInfo> MouseMoveEvent() const;

    rx::Observable<MouseEnterInfo> MouseEnterEvent() const;
    rx::Observable<MouseLeaveInfo> MouseLeaveEvent() const;
    rx::Observable<MouseHoverInfo> MouseHoverEvent() const;

    rx::Observable<PreMouseDownInfo> PreMouseDownEvent() const;
    rx::Observable<MouseDownInfo> MouseDownEvent() const;

    rx::Observable<PreMouseUpInfo> PreMouseUpEvent() const;
    rx::Observable<MouseUpInfo> MouseUpEvent() const;

    rx::Observable<DoubleClickInfo> DoubleClickEvent() const;

    rx::Observable<PreMouseWheelInfo> PreMouseWheelEvent() const;
    rx::Observable<MouseWheelInfo> MouseWheelEvent() const;

    rx::Observable<PreKeyDownInfo> PreKeyDownEvent() const;
    rx::Observable<KeyDownInfo> KeyDownEvent() const;

    rx::Observable<PreKeyUpInfo> PreKeyUpEvent() const;
    rx::Observable<KeyUpInfo> KeyUpEvent() const;

    rx::Observable<PreCharInputInfo> PreCharInputEvent() const;
    rx::Observable<CharInputInfo> CharInputEvent() const;

    rx::Observable<PreSysKeyDownInfo> PreSysKeyDownEvent() const;
    rx::Observable<SysKeyDownInfo> SysKeyDownEvent() const;

    rx::Observable<PreSysKeyUpInfo> PreSysKeyUpEvent() const;
    rx::Observable<SysKeyUpInfo> SysKeyUpEvent() const;

    rx::Observable<PreSysCharInputInfo> PreSysCharInputEvent() const;
    rx::Observable<SysCharInputInfo> SysCharInputEvent() const;

    rx::Observable<ParentChangedInfo> ParentChangedEvent() const;
    rx::Observable<WindowChangedInfo> WindowChangedEvent() const;

protected:
    void InvokeInitialize() override;
    void InvokeParse() override;

    void Initialize() override;

#pragma region Protected Geometry Management
    /**
    @name Protected Geometry Management
    @{
    */

    /**
    Called after the position of the control is changed.

    @param event_info
        Information of the event.

    @details
        The default implementation of this method raises `PositionChangedEvent()`. Derived classes
        should call the same method of base class if they override this method. 
    */
    virtual void OnPositionChanged(const PositionChangedInfo& event_info);

    /**
    Called after the size of the control is changed.

    @param event_info
        Information of the event.

    @details
        The default implementation of this method raises `SizeChangedEvent()`. Derived classes
        should call the same method of base class if they override this method. 
    */
    virtual void OnSizeChanged(const SizeChangedInfo& event_info);

    /**
    Called after the rectangle of the control is changed.

    @param event_info
        Information of the event.

    @details
        The default implementation of this method raises `RectChangedEvent()`. Derived classes
        should call the same method of base class if they override this method. 
    */
    virtual void OnRectChanged(const RectChangedInfo& event_info);

    /**}@*/
#pragma endregion

    virtual void UpdateStyle();
    virtual void OnStyleUpdate(const StyleUpdateInfo& event_info);

    /**
     Paint the control.

     @param canvas
        The canvas used to paint.

     @param dirty_rect
        The rectangle area needed to be repainted in the control.

     Derived classes can override this method to paint the control.
     */
    virtual void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const;

    /**
     Release the renderer-dependent resources.

     This method is called when renderer-dependent resources need to be released, 
     if they are being cached.

     Derived classes must call the same method of base class.
     */
    virtual void ReleaseRendererResources();

    /**
     Layout the control's children.

     @param previous_size
        The previous size of the control.

     Derived classes may not call the same method of base class if they do the layout 
     by themself.
     */
    virtual void Layout(const zaf::Size& previous_size);

    /**
     Require the control to relayout its children.
     */
    void RequestLayout();

    void AutoResizeToPreferredSize();

    virtual zaf::Size CalculatePreferredContentSize(const zaf::Size& bound_size) const;

    void RaiseContentChangedEvent();
    void RaiseDoubleClickEvent(const Point& position);

    virtual std::optional<HitTestResult> HitTest(const HitTestMessage& message);

    /**
    Handles mouse cursor changing event. This method is called when the mouse is over current 
    control and the window receives WM_SETCURSOR message.

    The default implementation raises MouseCursorChangingEvent. Derived classes should call the
    same method of base class.
    */
    virtual void OnMouseCursorChanging(const MouseCursorChangingInfo& event_info);

    virtual void OnPreMouseMove(const PreMouseMoveInfo& event_info);
    virtual void OnMouseMove(const MouseMoveInfo& event_info);

    virtual void OnMouseEnter(const MouseEnterInfo& event_info);
    virtual void OnMouseLeave(const MouseLeaveInfo& event_info);
    virtual void OnMouseHover(const MouseHoverInfo& event_info);

    virtual void OnPreMouseDown(const PreMouseDownInfo& event_info);
    virtual void OnMouseDown(const MouseDownInfo& event_info);

    virtual void OnPreMouseUp(const PreMouseUpInfo& event_info);
    virtual void OnMouseUp(const MouseUpInfo& event_info);

    virtual void OnDoubleClick(const DoubleClickInfo& event_info);

    virtual void OnPreMouseWheel(const PreMouseWheelInfo& event_info);
    virtual void OnMouseWheel(const MouseWheelInfo& event_info);

    /**
    Handles mouse captured event. This method is called after the control calling CaptureMouse() 
    and capturing mouse successfully.

    The default implementation raises MouseCapturedEvent. Derived classes should call the same 
    method of base class.
    */
    virtual void OnMouseCaptured(const MouseCapturedInfo& event_info);

    /**
    Handles mouse released event. This method is called after the control releasing mouse capture 
    with ReleaseMouse(), or losing mouse capture passively.

    The default implementation raises MouseReleasedEvent. Derived classes should call the same 
    method of base class.
    */
    virtual void OnMouseReleased(const MouseReleasedInfo& event_info);

    virtual void OnPreKeyDown(const PreKeyDownInfo& event_info);
    virtual void OnKeyDown(const KeyDownInfo& event_info);

    virtual void OnPreKeyUp(const PreKeyUpInfo& event_info);
    virtual void OnKeyUp(const KeyUpInfo& event_info);

    virtual void OnPreCharInput(const PreCharInputInfo& event_info);
    virtual void OnCharInput(const CharInputInfo& event_info);

    virtual void OnPreSysKeyDown(const PreSysKeyDownInfo& event_info);
    virtual void OnSysKeyDown(const SysKeyDownInfo& event_info);

    virtual void OnPreSysKeyUp(const PreSysKeyUpInfo& event_info);
    virtual void OnSysKeyUp(const SysKeyUpInfo& event_info);

    virtual void OnPreSysCharInput(const PreSysCharInputInfo& event_info);
    virtual void OnSysCharInput(const SysCharInputInfo& event_info);

    virtual void OnPreFocusGained(const PreFocusGainedInfo& event_info);
    virtual void OnFocusGained(const FocusGainedInfo& event_info);

    virtual void OnPreFocusLost(const PreFocusLostInfo& event_info);
    virtual void OnFocusLost(const FocusLostInfo& event_info);

    virtual void OnIMEStartComposition(const IMEStartCompositionInfo& event_info);
    virtual void OnIMEComposition(const IMECompositionInfo& event_info);
    virtual void OnIMEEndComposition(const IMEEndCompositionInfo& event_info);

    /**
    Called when the visibility of the control itself has changed.

    The default implementation raises IsVisibleChangedEvent. Derived classes must call the same
    method of base class if they override it.
    */
    virtual void OnIsVisibleChanged();

    /**
    Called when the enabled state of the control itself has changed.

    The default implementation raises IsEnabledChangedEvent. Derived classes must call the same
    method of base class if they override it.
    */
    virtual void OnIsEnabledChanged();

    /**
    Called when the selection state of the control itself has changed.

    The default implementation raises IsSelectedChangedEvent. Derived classes must call the same
    method of base class if they override it.
    */
    virtual void OnIsSelectedChanged();

    virtual void OnDPIChanged();

    virtual void OnParentChanged(const ParentChangedInfo& event_info);
    virtual void OnWindowChanged(const WindowChangedInfo& event_info);

private:
    friend class Caret;
    friend class Window;
    friend class internal::ControlEventInvokerBinder;
    friend class internal::ControlGeometryFacet;
    friend class internal::WindowControlFacet;
    friend class internal::WindowFocusedControlManager;
    friend class internal::WindowKeyboardFacet;
    friend class internal::WindowLifecycleFacet;
    friend class internal::WindowMouseFacet;
    friend class internal::WindowRenderFacet;

    void SetWindow(const std::shared_ptr<zaf::Window>& window) noexcept;

    void SetIsMouseOverByWindow(bool is_mouse_over);
    void SetIsFocusedByWindow(bool is_focused);
    void IsCapturingMouseChanged(bool is_capturing_mouse);

private:
    friend class internal::ControlUpdateLock;

    void EndUpdate();

private:
    /**
    The entry point to repaint the control.

    @param canvas
        The Canvas used to paint the control.

    @param dirty_rect
        The rect that needs to be repainted, in the control's coordinate space.
    */
    void Repaint(Canvas& canvas, const zaf::Rect& dirty_rect);
    bool HandleUpdateStyle();
    void RepaintUsingCachedPainting(Canvas& canvas, const zaf::Rect& dirty_rect);
    void RepaintControl(
        Canvas& canvas,
        const zaf::Rect& dirty_rect,
        bool need_clear,
        bool update_style);
    void RepaintChildren(Canvas& canvas, const zaf::Rect& dirty_rect, bool update_style);
    void RecalculateCachedPaintingRect(const zaf::Rect& repaint_rect);
    void ReleaseCachedPaintingRenderer();
    void DrawBackgroundImage(Canvas& canvas, const zaf::Rect& background_rect) const;

    void SetParent(const std::shared_ptr<Control>& parent);
    void InnerRemoveChild(const std::shared_ptr<Control>& child, bool set_parent_to_null);

    /**
     Called when a child's rect has changed.
     */
    void OnChildRectChanged(const std::shared_ptr<Control>& child, const zaf::Rect& previous_rect);

    std::shared_ptr<Control> InnerFindChildAtPosition(
        const Point& position, 
        bool recursively) const;

    void RequestLayout(const zaf::Size& previous_size);

    void SetInteractiveProperty(bool new_value, bool& property_value, void(Control::*notification)());

    bool HandleDoubleClickOnMouseDown(const Point& position);

    void RaiseWindowChangedEvent(const std::shared_ptr<zaf::Window>& previous_window);
    void RaiseDPIChangedEvent();

    std::shared_ptr<internal::InspectorPort> GetInspectorPort() const;

private:
    std::unique_ptr<internal::ControlGeometryFacet> geometry_facet_;

    std::weak_ptr<zaf::Window> window_;
    std::weak_ptr<Control> parent_;
    std::vector<std::shared_ptr<Control>> children_;

    std::weak_ptr<internal::ControlUpdateLock> update_lock_;
    std::unique_ptr<internal::ControlUpdateState> update_state_;

    bool need_update_style_{};
    bool is_updating_style_{};

    bool is_cached_painting_enabled_{};
    d2d::BitmapRenderer cached_renderer_;
    zaf::Rect valid_cached_renderer_rect_;

    std::shared_ptr<zaf::Layouter> layouter_;
    bool is_auto_resizing_{ false };
    bool is_layouting_{ false };

    bool is_mouse_over_;
    bool is_capturing_mouse_;
    bool is_focused_;
    bool can_focus_{ false };
    bool can_double_click_{ false };
    bool is_enabled_;
    bool is_visible_;
    bool is_selected_{};
    bool can_tab_stop_{ true };
    std::optional<std::size_t> tab_index_;

    std::uint32_t last_mouse_down_time_{};
    Point last_mouse_down_position_;

    internal::ColorField background_color_field_;
    internal::ColorField border_color_field_;

    ImageLayout background_image_layout_{ ImageLayout::None };
    ImagePicker background_image_picker_;

    std::wstring name_;
    std::wstring tooltip_;

    Event<StyleUpdateInfo> style_update_event_;
    Event<RectChangedInfo> rect_changed_event_;
    Event<PositionChangedInfo> position_changed_event_;
    Event<SizeChangedInfo> size_changed_event_;
    Event<IsVisibleChangedInfo> is_visible_changed_event_;
    Event<IsEnabledChangedInfo> is_enabled_changed_event_;
    Event<IsSelectedChangedInfo> is_selected_changed_event_;
    Event<PreFocusGainedInfo> pre_focus_gained_event_;
    Event<FocusGainedInfo> focus_gained_event_;
    Event<PreFocusLostInfo> pre_focus_lost_event_;
    Event<FocusLostInfo> focus_lost_event_;
    Event<MouseCursorChangingInfo> mouse_cursor_changing_event_;
    Event<PreMouseMoveInfo> pre_mouse_move_event_;
    Event<MouseMoveInfo> mouse_move_event_;
    Event<MouseEnterInfo> mouse_enter_event_;
    Event<MouseLeaveInfo> mouse_leave_event_;
    Event<MouseHoverInfo> mouse_hover_event_;
    Event<PreMouseDownInfo> pre_mouse_down_event_;
    Event<MouseDownInfo> mouse_down_event_;
    Event<PreMouseUpInfo> pre_mouse_up_event_;
    Event<MouseUpInfo> mouse_up_event_;
    Event<PreMouseWheelInfo> pre_mouse_wheel_event_;
    Event<MouseWheelInfo> mouse_wheel_event_;
    Event<DoubleClickInfo> double_click_event_;
    Event<PreKeyDownInfo> pre_key_down_event_;
    Event<KeyDownInfo> key_down_event_;
    Event<PreKeyUpInfo> pre_key_up_event_;
    Event<KeyUpInfo> key_up_event_;
    Event<PreCharInputInfo> pre_char_input_event_;
    Event<CharInputInfo> char_input_event_;
    Event<PreSysKeyDownInfo> pre_sys_key_down_event_;
    Event<SysKeyDownInfo> sys_key_down_event_;
    Event<PreSysKeyUpInfo> pre_sys_key_up_event_;
    Event<SysKeyUpInfo> sys_key_up_event_;
    Event<PreSysCharInputInfo> pre_sys_char_input_event_;
    Event<SysCharInputInfo> sys_char_input_event_;
    Event<ParentChangedInfo> parent_changed_event_;
    Event<WindowChangedInfo> window_changed_event_;
    Event<MouseCapturedInfo> mouse_captured_event_;
    Event<MouseReleasedInfo> mouse_released_event_;
};

}