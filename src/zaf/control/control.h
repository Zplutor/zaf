#pragma once

#include <Windows.h>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <zaf/base/event/event.h>
#include <zaf/control/anchor.h>
#include <zaf/control/event/double_click_info.h>
#include <zaf/control/event/focus_event_info.h>
#include <zaf/control/event/ime_event_infos.h>
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
#include <zaf/rx/subscription_host.h>

namespace zaf {
namespace internal {
class ControlEventInvokerBinder;
class WindowFocusedControlManager;
class ControlUpdateLock;
class ControlUpdateState;
class InspectorPort;
}

class Canvas;
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
    public SubscriptionHost, 
    public std::enable_shared_from_this<Control> {

public:
    ZAF_OBJECT;

public:
    Control();
    virtual ~Control();

    Control(const Control&) = delete;
    Control& operator=(const Control&) = delete;

    [[nodiscard]]
    ControlUpdateGuard BeginUpdate();

    /**
    Gets the control's rectangle area in the window's coordinate space.

    @return 
        The rectangle area of the control in the window's coordinate space. If the control is not
        in a window, std::nullopt is returned.
    */
    std::optional<zaf::Rect> RectInWindow() const noexcept;

    /**
    Gets the control's rectangle area in its own coordinate space.

    @return 
        The rectangle area of the control in its own coordinate space, where the position is always 
        zero.
    */
    zaf::Rect RectInSelf() const noexcept;

    /**
    Gets the control's rectangle area in its container's coordinate space.

    @return
        The rectangle area of the control in its container's coordinate space.

    @remark
        If the control is the root control of a window, the rect is expressed in the coordinate 
        space of the window's client area; otherwise the rect is expressed in the coordinate space
        of its parent's content area.

        The default rect of a control is empty.
    */
    const zaf::Rect& Rect() const {
        return rect_;
    }

    /**
    Sets the control's rectangle area in its container's coordinate space.

    @param rect
         The rect to be set.
    */
    void SetRect(const zaf::Rect& rect);

    /**
    Gets the control's position in the window's coordinate space.

    @return
        The position of the control in the window's coordinate space. If the control is not in a 
        window, std::nullopt is returned.
    */
    std::optional<zaf::Point> PositionInWindow() const noexcept;

    /**
     Get the control's position which is related to the coordinate system of 
     parent content rect.
     */
    const Point& Position() const {
        return rect_.position;
    }

    /**
     Set the control's position.

     See also Position.
     */
    void SetPosition(const Point& position) {
        SetRect(zaf::Rect(position, Rect().size));
    }

    float X() const {
        return Position().x;
    }

    void SetX(float x) {
        SetPosition(Point(x, Y()));
    }

    float Y() const {
        return Position().y;
    }

    void SetY(float y) {
        SetPosition(Point(X(), y));
    }

    /**
     Get the control's size.
     */
    const zaf::Size& Size() const {
        return rect_.size;
    }

    /**
     Set the control's size;
     */
    void SetSize(const zaf::Size& size) {
        SetRect(zaf::Rect(Rect().position, size));
    }

    /**
     Get the control's width.
     */
    float Width() const {
        return rect_.size.width;
    }

    /**
     Set the control's width.
     */
    void SetWidth(float width) {
        SetSize(zaf::Size(width, Height()));
    }

    /**
     Get the control's minimum width.

     The default value is 0.
     */
    float MinWidth() const;

    /**
     Set the control's minimum width.
     */
    void SetMinWidth(float min_width);

    /**
     Get the control's maximum width.

     The default value is the maximum value of float.
     */
    float MaxWidth() const;

    /**
     Set the control's maximum width.
     */
    void SetMaxWidth(float max_width);

    void SetFixedWidth(float width);
    void SetFixedHeight(float height);
    void SetFixedSize(const zaf::Size& size);

    /**
     Get the control's height.
     */
    float Height() const {
        return rect_.size.height;
    }

    /**
     Set the control's height.
     */
    void SetHeight(float height) {
        SetSize(zaf::Size(Width(), height));
    }

    /**
     Get the control's minimum height.

     The default value is 0.
     */
    float MinHeight() const;

    /**
     Set the control's minimum height.
     */
    void SetMinHeight(float min_height);

    /**
     Get the control's maximum height.

     The default value is the maximum value of float.
     */
    float MaxHeight() const;

    /**
     Set the control's maximum height.
     */
    void SetMaxHeight(float max_height);

    zaf::Size CalculatePreferredSize() const;
    zaf::Size CalculatePreferredSize(const zaf::Size& bound_size) const;

    bool AutoWidth() const;
    void SetAutoWidth(bool value);

    bool AutoHeight() const;
    void SetAutoHeight(bool value);

    void SetAutoSize(bool value);

    float ApplyWidthLimit(float width) const;
    float ApplyHeightLimit(float height) const;
    zaf::Size ApplySizeLimit(const zaf::Size& size) const;

    /**
     Get the control's anchor.

     The default value is Anchor::None.
     */
    Anchor Anchor() const;

    /**
     Set the control's anchor.
     */
    void SetAnchor(zaf::Anchor anchor);

    const Frame& Margin() const {
        return margin_;
    }

    void SetMargin(const Frame& margin);

    const Frame& Border() const {
        return border_;
    }

    void SetBorder(const Frame& border) {
        border_ = border;
        NeedRepaint();
        NeedRelayout();
    }

    const Frame& Padding() const {
        return padding_;
    }

    void SetPadding(const Frame& padding) {
        padding_ = padding;
        NeedRelayout();
    }

    /**
    Gets the control's content's rectangle area in the window's coordinate space.

    @return
        The rectangle area of the control's content in the window's coordinate space. If the 
        control is not in a window, std::nullopt is returned.
    */
    std::optional<zaf::Rect> ContentRectInWindow() const noexcept;

    /**
     Get the control's content rect, related to its coordinate system.
     */
    zaf::Rect ContentRect() const;

    /**
     Get the control's content size.
     */
    zaf::Size ContentSize() const {
        return ContentRect().size;
    }

    std::shared_ptr<Image> BackgroundImage() const {
        return BackgroundImagePicker()(*this);
    }

    ImagePicker BackgroundImagePicker() const;

    void SetBackgroundImage(const std::shared_ptr<Image>& image) {
        SetBackgroundImagePicker(CreateImagePicker(image));
    }

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

    bool HasChildren() const {
        return !children_.empty();
    }

    std::size_t ChildCount() const {
        return children_.size();
    }

    const std::shared_ptr<Control>& GetChildAtIndex(std::size_t index) const {
        return children_[index];
    }

    /**
     Get the control's children.
     */
    const std::vector<std::shared_ptr<Control>>& Children() const {
        return children_;
    }

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

    std::shared_ptr<Control> FindChildAtPosition(const Point& position) const {
        return InnerFindChildAtPosition(position, false);
    }

    std::shared_ptr<Control> FindChildAtPositionRecursively(const Point& position) const {
        return InnerFindChildAtPosition(position, true);
    }

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
    std::shared_ptr<Control> Parent() const {
        return parent_.lock();
    }

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

    Observable<IsVisibleChangedInfo> IsVisibleChangedEvent() const;

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

    Observable<IsEnabledChangedInfo> IsEnabledChangedEvent() const;

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

    Observable<IsSelectedChangedInfo> IsSelectedChangedEvent() const;

    /**
     Get a value indicating that whether the control itself is under mouse cursor.
     */
    bool IsMouseOver() const {
        return is_mouse_over_;
    }

    /**
    Indicates whether the control or any of its children is under mouse cursor.
    */
    bool ContainsMouse() const;

    /**
     Get a value indicating that whether the control can be focused.

     The default value is false.
     */
    bool CanFocused() const {
        return can_focused_;
    }

    /**
     Set a value indicating that whether the control can be focused.

     See also CanFocused.
     */
    void SetCanFocused(bool can_focused) {
        can_focused_ = can_focused;
    }

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
    bool IsFocused() const {
        return is_focused_;
    }

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

    bool IsCachedPaintingEnabled() const {
        return is_cached_painting_enabled_;
    }

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
    bool IsCapturingMouse() const noexcept {
        return is_capturing_mouse_;
    }

    /**
    Gets mouse captured event. This event is raised after the control calling CaptureMouse() and 
    capturing mouse successfully.
    */
    Observable<MouseCapturedInfo> MouseCapturedEvent() const;

    /**
     Release the mouse.
     */
    void ReleaseMouse();

    /**
    Gets mouse released event. This event is raised after the control releasing mouse capture with 
    ReleaseMouse(), or losing mouse capture passively.
    */
    Observable<MouseReleasedInfo> MouseReleasedEvent() const;

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
    virtual bool AcceptKeyMessage(const KeyMessage& message) {
        return false;
    }

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

    Observable<StyleUpdateInfo> StyleUpdateEvent() const;

    /**
     Get rect change event.

     This event is raised when the control's rect is changed.
     */
    Observable<RectChangedInfo> RectChangedEvent() const;
    Observable<PositionChangedInfo> PositionChangedEvent() const;
    Observable<SizeChangedInfo> SizeChangedEvent() const;

    Observable<PreFocusGainedInfo> PreFocusGainedEvent() const;
    Observable<FocusGainedInfo> FocusGainedEvent() const;

    Observable<PreFocusLostInfo> PreFocusLostEvent() const;
    Observable<FocusLostInfo> FocusLostEvent() const;

    Observable<MouseCursorChangingInfo> MouseCursorChangingEvent() const;

    Observable<PreMouseMoveInfo> PreMouseMoveEvent() const;
    Observable<MouseMoveInfo> MouseMoveEvent() const;

    Observable<MouseEnterInfo> MouseEnterEvent() const;
    Observable<MouseLeaveInfo> MouseLeaveEvent() const;
    Observable<MouseHoverInfo> MouseHoverEvent() const;

    Observable<PreMouseDownInfo> PreMouseDownEvent() const;
    Observable<MouseDownInfo> MouseDownEvent() const;

    Observable<PreMouseUpInfo> PreMouseUpEvent() const;
    Observable<MouseUpInfo> MouseUpEvent() const;

    Observable<DoubleClickInfo> DoubleClickEvent() const;

    Observable<PreMouseWheelInfo> PreMouseWheelEvent() const;
    Observable<MouseWheelInfo> MouseWheelEvent() const;

    Observable<PreKeyDownInfo> PreKeyDownEvent() const;
    Observable<KeyDownInfo> KeyDownEvent() const;

    Observable<PreKeyUpInfo> PreKeyUpEvent() const;
    Observable<KeyUpInfo> KeyUpEvent() const;

    Observable<PreCharInputInfo> PreCharInputEvent() const;
    Observable<CharInputInfo> CharInputEvent() const;

    Observable<PreSysKeyDownInfo> PreSysKeyDownEvent() const;
    Observable<SysKeyDownInfo> SysKeyDownEvent() const;

    Observable<PreSysKeyUpInfo> PreSysKeyUpEvent() const;
    Observable<SysKeyUpInfo> SysKeyUpEvent() const;

    Observable<PreSysCharInputInfo> PreSysCharInputEvent() const;
    Observable<SysCharInputInfo> SysCharInputEvent() const;

    Observable<ParentChangedInfo> ParentChangedEvent() const;
    Observable<WindowChangedInfo> WindowChangedEvent() const;

protected:
    void InvokeInitialize() override;
    void InvokeParse() override;

    void Initialize() override;

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

     @param previous_rect
        The previous rect of the control.

     Derived classes may not call the same method of base class if they do the layout 
     by themself.
     */
    virtual void Layout(const zaf::Rect& previous_rect);

    /**
     Require the control to relayout its children.
     */
    void NeedRelayout();

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
    Handles rect changed event. This method is called after the rect of the control changed.

    The default implementation raises RectChangedEvent, calls OnPositionChanged() if the position
    is changed, and calls OnSizeChanged() if the size is changed.
    */
    virtual void OnRectChanged(const RectChangedInfo& event_info);

    /**
    Handles position changed event. This method is called after the position of the control
    changed.

    The default implementation raises SizeChangedEvent.
    */
    virtual void OnPositionChanged(const PositionChangedInfo& event_info);

    /**
    Handles size changed event. This method is called after the size of the control changed.

    The default implementation raises SizeChangedEvent.
    */
    virtual void OnSizeChanged(const SizeChangedInfo& event_info);

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
    friend class internal::WindowFocusedControlManager;

    void SetWindow(const std::shared_ptr<zaf::Window>& window) {
        window_ = window;
    }

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

    void SetFixedWidthValue(float value);
    void SetFixedHeightValue(float value);
    void ApplyAutoSizeOnRectChanged(zaf::Size& new_size);
    void AutoResizeToPreferredSize();
    zaf::Size CalculatePreferredSizeForAutoSize(const zaf::Size& control_size) const;

    /**
     Called when a child's rect has changed.
     */
    void OnChildRectChanged(const std::shared_ptr<Control>& child, const zaf::Rect& previous_rect);

    std::shared_ptr<Control> InnerFindChildAtPosition(
        const Point& position, 
        bool recursively) const;

    void NeedRelayout(const zaf::Rect& previous_rect);

    void SetInteractiveProperty(bool new_value, bool& property_value, void(Control::*notification)());

    bool HandleDoubleClickOnMouseDown(const Point& position);

    void RaiseWindowChangedEvent(const std::shared_ptr<zaf::Window>& previous_window);
    void RaiseDPIChangedEvent();

    std::shared_ptr<internal::InspectorPort> GetInspectorPort() const;

private:
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

    zaf::Anchor anchor_{ zaf::Anchor::None };
    bool auto_width_{};
    bool auto_height_{};
    float min_width_{};
    float max_width_{ (std::numeric_limits<float>::max)() };
    float min_height_{};
    float max_height_{ (std::numeric_limits<float>::max)() };
    std::shared_ptr<zaf::Layouter> layouter_;
    bool is_auto_resizing_{ false };
    bool is_layouting_{ false };

    bool is_mouse_over_;
    bool is_capturing_mouse_;
    bool is_focused_;
    bool can_focused_;
    bool can_double_click_{ false };
    bool is_enabled_;
    bool is_visible_;
    bool is_selected_{};
    bool can_tab_stop_{ true };
    std::optional<std::size_t> tab_index_;

    zaf::Rect rect_;
    Frame margin_;
    Frame border_;
    Frame padding_;

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