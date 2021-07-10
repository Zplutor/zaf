#pragma once

#include <Windows.h>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <zaf/control/anchor.h>
#include <zaf/control/color_picker.h>
#include <zaf/control/image_layout.h>
#include <zaf/control/image_picker.h>
#include <zaf/control/layout/layouter.h>
#include <zaf/control/control_update_guard.h>
#include <zaf/graphic/renderer/bitmap_renderer.h>
#include <zaf/graphic/color.h>
#include <zaf/graphic/frame.h>
#include <zaf/graphic/rect.h>
#include <zaf/object/object.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/subscription_host.h>
#include <zaf/serialization/property_map.h>

namespace zaf {
namespace internal {
class ControlUpdateLock;
class ControlUpdateState;
class InspectorPort;
}

class Canvas;
class CharMessage;
class ControlClickInfo;
class ControlDoubleClickInfo;
class ControlFocusChangeInfo;
class ControlMouseEnterInfo;
class ControlMouseLeaveInfo;
class ControlRectChangeInfo;
class KeyMessage;
class Message;
class MouseMessage;
class MouseWheelMessage;
class Renderer;
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
    ZAF_DECLARE_TYPE

public:
    Control();
    virtual ~Control();

    Control(const Control&) = delete;
    Control& operator=(const Control&) = delete;

    [[nodiscard]]
    ControlUpdateGuard BeginUpdate();

    /**
     Get the control's absolute rect which is related to the coordinate system of 
     window's content rect.

     If the control is not yet placed in a window, am empty rect is returned.
     */
    zaf::Rect GetAbsoluteRect() const;

    /**
     Get the control's rect which is related to the coordinate system of parent's
     content rect.

     The default rect is empty.
     */
    const zaf::Rect& Rect() const {
        return rect_;
    }

    /**
     Set the control's rect.

     See also Rect.
     */
    void SetRect(const zaf::Rect& rect);

    /**
     Get the control's position which is related to the coordinate system of 
     parent content rect.
     */
    const Point& GetPosition() const {
        return rect_.position;
    }

    /**
     Set the control's position.

     See also GetPosition.
     */
    void SetPosition(const Point& position) {
        SetRect(zaf::Rect(position, Rect().size));
    }

    float GetX() const {
        return GetPosition().x;
    }

    void SetX(float x) {
        SetPosition(Point(x, GetY()));
    }

    float GetY() const {
        return GetPosition().y;
    }

    void SetY(float y) {
        SetPosition(Point(GetX(), y));
    }

    /**
     Get the control's size.
     */
    const Size& GetSize() const {
        return rect_.size;
    }

    /**
     Set the control's size;
     */
    void SetSize(const Size& size) {
        SetRect(zaf::Rect(Rect().position, size));
    }

    /**
     Get the control's width.
     */
    float GetWidth() const {
        return rect_.size.width;
    }

    /**
     Set the control's width.
     */
    void SetWidth(float width) {
        SetSize(zaf::Size(width, GetHeight()));
    }

    /**
     Get the control's minimum width.

     The default value is 0.
     */
    float GetMinWidth() const;

    /**
     Set the control's minimum width.
     */
    void SetMinWidth(float min_width);

    /**
     Get the control's maximum width.

     The default value is the maximum value of float.
     */
    float GetMaxWidth() const;

    /**
     Set the control's maximum width.
     */
    void SetMaxWidth(float max_width);

    void SetFixedWidth(float width) {
        SetMaxWidth(width);
        SetMinWidth(width);
    }

    void SetFixedHeight(float height) {
        SetMaxHeight(height);
        SetMinHeight(height);
    }

    void SetFixedSize(const Size& size) {
        SetFixedWidth(size.width);
        SetFixedHeight(size.height);
    }

    /**
     Get the control's height.
     */
    float GetHeight() const {
        return rect_.size.height;
    }

    /**
     Set the control's height.
     */
    void SetHeight(float height) {
        SetSize(zaf::Size(GetWidth(), height));
    }

    /**
     Get the control's minimum height.

     The default value is 0.
     */
    float GetMinHeight() const;

    /**
     Set the control's minimum height.
     */
    void SetMinHeight(float min_height);

    /**
     Get the control's maximum height.

     The default value is the maximum value of float.
     */
    float GetMaxHeight() const;

    /**
     Set the control's maximum height.
     */
    void SetMaxHeight(float max_height);

    Size GetPreferredSize() const;

    void ResizeToPreferredSize();

    bool AutoWidth() const;
    void SetAutoWidth(bool value);

    bool AutoHeight() const;
    void SetAutoHeight(bool value);

    void SetAutoSize(bool value);

    /**
     Get the control's anchor.

     The default value is Anchor::None.
     */
    Anchor GetAnchor() const;

    /**
     Set the control's anchor.
     */
    void SetAnchor(Anchor anchor);

    const Frame& GetMargin() const {
        return margin_;
    }

    void SetMargin(const Frame& margin);

    void SetMargin(float margin_thickness) {
        SetMargin(Frame(margin_thickness, margin_thickness, margin_thickness, margin_thickness));
    }

    const Frame& GetBorder() const {
        return border_;
    }

    void SetBorder(const Frame& border) {
        border_ = border;
        NeedRepaint();
        NeedRelayout();
    }

    void SetBorder(float border_thickness) {
        SetBorder(Frame(border_thickness, border_thickness, border_thickness, border_thickness));
    }

    const Frame& GetPadding() const {
        return padding_;
    }

    void SetPadding(const Frame& padding) {
        padding_ = padding;
        NeedRelayout();
    }

    void SetPadding(float padding_thickness) {
        SetPadding(Frame(padding_thickness, padding_thickness, padding_thickness, padding_thickness));
    }

    /**
     Get the control's content rect, related to its coordinate system.
     */
    zaf::Rect GetContentRect() const;

    /**
     Get the control's content size.
     */
    Size GetContentSize() const {
        return GetContentRect().size;
    }

    std::shared_ptr<Image> GetBackgroundImage() const {
        return GetBackgroundImagePicker()(*this);
    }

    ImagePicker GetBackgroundImagePicker() const;

    void SetBackgroundImage(const std::shared_ptr<Image>& image) {
        SetBackgroundImagePicker(CreateImagePicker(image));
    }

    void SetBackgroundImagePicker(const ImagePicker& image_picker);

    ImageLayout GetBackgroundImageLayout() const;
    void SetBackgroundImageLayout(ImageLayout image_layout);

    /**
     Get background color.
     */
    Color GetBackgroundColor() const {
        return GetBackgroundColorPicker()(*this);
    }

    /**
     Get the color picker of background.
     */
    ColorPicker GetBackgroundColorPicker() const;

    /**
     Set background color.
     */
    void SetBackgroundColor(const Color& color) {
        SetBackgroundColorPicker(CreateColorPicker(color));
    }

    /**
     Set the color picker of background.
     */
    void SetBackgroundColorPicker(const ColorPicker& color_picker);

    /**
     Get border color.
     */
    Color GetBorderColor() const {
        return GetBorderColorPicker()(*this);
    }

    /**
     Get the color picker of border color.
     */
    ColorPicker GetBorderColorPicker() const;

    /**
     Set border color.
     */
    void SetBorderColor(const Color& color) {
        SetBorderColorPicker(CreateColorPicker(color));
    }

    /**
     Set the color picker of border.
     */
    void SetBorderColorPicker(const ColorPicker& color_picker);

    /**
     Get the layouter.
     */
    std::shared_ptr<Layouter> GetLayouter() const;

    /**
     Set the layouter.

     Setting the layouter would causes the control to relayout immediately.
     */
    void SetLayouter(const std::shared_ptr<Layouter>& layouter);

    bool HasChildren() const {
        return !children_.empty();
    }

    std::size_t GetChildCount() const {
        return children_.size();
    }

    const std::shared_ptr<Control>& GetChildAtIndex(std::size_t index) const {
        return children_[index];
    }

    /**
     Get the control's children.
     */
    const std::vector<std::shared_ptr<Control>>& GetChildren() const {
        return children_;
    }

    /**
     Add specified child to the control.
     */
    void AddChild(const std::shared_ptr<Control>& child);

    /**
     Add specified children to the control.
     */
    void AddChildren(const std::vector<std::shared_ptr<Control>>& children);

    /**
     Remoe specified child from the control.
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
     Determinte whether the control is the direct parent of specified control.
     */
    bool IsParentOf(const std::shared_ptr<Control>& child) const;

    /**
     Determinte whether the control is the ancestor of specified control.
     */
    bool IsAncestorOf(const std::shared_ptr<Control>& child) const;

    /**
     Get the control's parent.

     Return nullptr if the control does not have parent.
     */
    std::shared_ptr<Control> GetParent() const {
        return parent_.lock();
    }

    /**
     Get the control's name.

     The name is used to identify a child in control. Multiple 
     children may have the same name. The default name is empty.
     */
    std::wstring GetName() const;

    /**
     Set the control's name.

     See also GetName.
     */
    void SetName(const std::wstring& name);

    /**
     Get the window where the control locates.

     Return nullptr if the control does not locates in any window.
     */
    std::shared_ptr<Window> GetWindow() const;

    /**
     Get a value indicating that whether the control is visible.

     The default value is true. Note that the return value is always 
     false if parent control is invisible.
     */
    bool IsVisible() const;

    bool IsSelfVisible() const;

    /**
     Set a value indicating that whether the control is visible.

     See also IsVisible. 
     */
    void SetIsVisible(bool is_visible);

    /**
     Get a value indicating that whether the control is enabled.

     The default value is true. Note that the return value is always
     false if parent control is disabled.
     */
    bool IsEnabled() const;

    /**
     Set a value indicating that whether the control is enabled.

     See also IsEnabled.
     */
    void SetIsEnabled(bool is_enabled);

    bool IsSelected() const;
    void SetIsSelected(bool is_selected);

    /**
     Get a value indicating that whether the control itself is hovered.
     */
    bool IsHovered() const {
        return is_hovered_;
    }

    /**
     Get a value indicating that whether one of the control's children is hovered.
     */
    bool IsHoveredIndirectly() const;

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
    std::optional<std::size_t> GetTabIndex() const;

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

    bool IsCachedPaintingEnabled() const {
        return is_cached_painting_enabled_;
    }

    void SetIsCachedPaintingEnabled(bool value);

    bool CanClick() const;
    void SetCanClick(bool can_click);

    bool CanDoubleClick() const;
    void SetCanDoubleClick(bool can_double_click);

    /**
     Capture the mouse.
     */
    void CaptureMouse();

    /**
     Release the mouse.
     */
    void ReleaseMouse();

    /**
     The the mouse position in control's coordinate.
     */
    const Point GetMousePosition() const;

    /**
     Determinte whether the control accepts specified key message.

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

    /**
     Get rect change event.

     This event is raised when the control's rect is changed.
     */
    Observable<ControlRectChangeInfo> RectChangeEvent();

    /**
     Get focus change event.

     This event is raised when the control's focus is changed.
     */
    Observable<ControlFocusChangeInfo> FocusChangeEvent();

    Observable<ControlMouseEnterInfo> MouseEnterEvent();
    Observable<ControlMouseLeaveInfo> MouseLeaveEvent();

    Observable<ControlClickInfo> ClickEvent();
    Observable<ControlDoubleClickInfo> DoubleClickEvent();

protected:
    void InvokeInitialize() override;

    /**
     Paint the control.

     @param canvas
        The canvas used to paint.

     @param dirty_rect
        The rectangle area needed to be repainted in the control.

     Derived classes can override this method to paint the control.
     */
    virtual void Paint(Canvas& canvas, const zaf::Rect& dirty_rect);

    /**
     Require the control to repaint.
     */
    void NeedRepaint();

    /**
     Require the control to repaint specified rectangle area.
     */
    void NeedRepaintRect(const zaf::Rect& rect);

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

    virtual Size GetPreferredContentSize() const;

    void RaiseContentChangedEvent();
    void RaiseClickEvent();
    void RaiseDoubleClickEvent();

    /**
     Get the mutable property map.
     */
    PropertyMap& GetPropertyMap() {
        return property_map_;
    }

    /**
     Get the immutable property map.
     */
    const PropertyMap& GetPropertyMap() const {
        return property_map_;
    }

    /**
     Get a value indicating that whether the control is capturing the mouse.
     */
    bool IsCapturingMouse() const {
        return is_capturing_mouse_;
    }

    virtual std::optional<HitTestResult> HitTest(const HitTestMessage& message);

    /**
     Change the mouse cursor.

     @param message
        Information about the WM_SETCURSOR message.

     @param is_changed
        An output parameter. If the cursor has been changed, it should be set to true.

     This method is called when a WM_SETCURSOR message is received. Derived classes should 
     call the same method of base class if they don't change the cursor.
     */
    virtual void ChangeMouseCursor(const Message& message, bool& is_changed);

    /**
     Process the mouse move notificaiton.

     @param position
        The mouse position in the control's coordinate.

     @param message
        Information about the WM_MOUSEMOVE message.

     @return
        Indicates that whether the notification has been processed by the control,
        and should not delivered to the system.

     This method is called when a WM_MOUSEMOVE message is received. Derived classes should 
     call the same method of base class if they don't process the notification.
     */
    virtual bool OnMouseMove(const Point& position, const MouseMessage& message);

    /**
     Process the mouse enter notification.

     @param entered_control
         The control which mouse enters. It is the current control or its children.

     This method is called when the mouse has entered the control. Derived classes should 
     call the same method of base class if they don't process the notification.
     */
    virtual void OnMouseEnter(const std::shared_ptr<Control>& entered_control);

    /**
     Process the mouse leave notification.

     @param entered_control
         The control which mouse leaves. It is the current control or its children.

     This method is called when the mouse has left the control. Derived classes should 
     call the same method of base class if they don't process the notifiction.
     */
    virtual void OnMouseLeave(const std::shared_ptr<Control>& leaved_control);

    /**
     Process the mouse down notification.

     @param position
        The mouse position in the control's coordinate.

     @param message
        Information about the mouse down message (e.g. WM_LBUTTONDOWN).

     @return
        Indicates that whether the notification has been processed by the control, 
        and should not delivered to the system. 

     This method is called when a mouse button is pressed within the control. Derived classes 
     should call the same method of base class if they don't process the notifiction.
     */
    virtual bool OnMouseDown(const Point& position, const MouseMessage& message);

    /**
     Process the mouse up notification.

     @param position
        The mouse position in the control's coordinate.

     @param message
        Information about the mouse up message (e.g. WM_LBUTTONUP).

     @return
        Indicates that whether the notification has been processed by the control,
        and should not delivered to the system.

     This method is called when a mouse button is released within the control. Derived classes 
     should call the same method of base class if they don't process the notifiction.
     */
    virtual bool OnMouseUp(const Point& position, const MouseMessage& message);

    virtual void OnClick();
    virtual void OnDoubleClick();

    /**
     Process the mouse wheel notification.

     @param position
        The mouse position in the control's coordinate.

     @param message
        Information about the mouse wheel message (e.g. WM_MOUSEWHEEL).

     @return
        Indicates that whether the notification has been processed by the control,
        and should not delivered to the system.   

     This method is called when the mouse wheel within the control. Derived classes
     should call the same method of base class if they don't process the notification.
     */
    virtual bool OnMouseWheel(const Point& position, const MouseWheelMessage& message);

    /**
     Process the mouse capture notification.

     This method is called when the control captured the mouse. Derived classes
     should call the same method of base class if they don't process the notification.
     */
    virtual void OnMouseCapture();

    /**
     Process the mouse release notification.

     This method is called when the control release the mouse. Derived classes
     should call the same method of base class if they don't process the notification.
     */
    virtual void OnMouseRelease();

    /**
     Process the key down notification.

     This method is called when a key is pressed at mean while the control is focused. Derived 
     classes should call the same method of base class if they don't process the notification.
     */
    virtual bool OnKeyDown(const KeyMessage& message);

    /**
     Process the key up notification.

     This method is called when a key is released at mean while the control is focused. Derived 
     classes should call the same method of base class if they don't process the notification.
     */
    virtual bool OnKeyUp(const KeyMessage& message);

    /**
     Process the char input notification.

     This method is called when a char is inputed at mean while the control is focused. Derived 
     classes should call the same method of base class if they don't process the notification.
     */
    virtual bool OnCharInput(const CharMessage& message);

    /**
     Process the focus gain notification.

     This method is called when the control gained the focus. Derived classes should call the 
     same method of base class if they don't process the notification.
     */
    virtual void OnFocusGain();

    /**
     Process the focus lose notification.

     This method is called when the control lost the focus. Derived classes should call the 
     same method of base class if they don't process the notification.
     */
    virtual void OnFocusLose();

    /**
     Process the rect change notification.

     This method is called when the control change its rect. Derived classes must call the 
     same method of base class.

     If the size is changed, this method is called before calling Layout method. You don't 
     need to call NeedRelayout method in this method, and you should layout children in Layout
     method instead of this method.
     */
    virtual void OnRectChanged(const zaf::Rect& previous_rect);

    /**
     Process the is visible change notification.

     This method is called when the control change its visibility. Derived classes must call
     the same method of base class.
     */
    virtual void OnIsVisibleChanged();

    /**
     Process the is enabled change notification.

     This method is called when the control is enabled or disabled. Derived classes must call 
     the same method of base class.
     */
    virtual void OnIsEnabledChanged();

    virtual void OnIsSelectedChanged();

private:
    friend class Window;

    void SetWindow(const std::shared_ptr<Window>& window) {
        window_ = window;
    }

    void IsHoveredChanged(bool is_hovered);
    void IsFocusedChanged(bool is_focused);
    void IsCapturingMouseChanged(bool is_capturing_mouse);

    void RouteHoverMessage(const Point& position, const MouseMessage& message);
    bool RouteMessage(const Point& position, const MouseMessage& message);
    bool InterpretMessage(const Point& position, const MouseMessage& message);

private:
    friend class internal::ControlUpdateLock;

    void EndUpdate();

private:
    /**
     The entry point to repaint the control.

     @param canvas
         Canvas used to paint the content of control.

     @param dirty_rect
         The rect in control's coordinate needed to repaint.
     */
    void Repaint(Canvas& canvas, const zaf::Rect& dirty_rect);

    void RepaintUsingCachedPainting(Canvas& canvas, const zaf::Rect& dirty_rect);
    void RepaintControl(Canvas& canvas, const zaf::Rect& dirty_rect, bool need_clear);
    void RecalculateCachedPaintingRect(const zaf::Rect& repaint_rect);
    void ReleaseCachedPaintingRenderer();
    void DrawBackgroundImage(Canvas& canvas, const zaf::Rect& background_rect);

    void SetParent(const std::shared_ptr<Control>& parent);

    void AutoResizeToPreferredSize();
    void InnerResizeToPreferredSize(bool resize_width, bool resize_height);

    /**
     Called when a child's rect has changed.
     */
    void ChildRectChanged(const std::shared_ptr<Control>& child, const zaf::Rect& previous_rect);

    /**
     Translate a point to which in parent's coordinate system.
     */
    Point ToParentPoint(const Point& point) {
        return Point(point.x + rect_.position.x, point.y + rect_.position.y);
    }

    /**
     Tanslate a point to which in specified child's coordinate system.
     */
    Point ToChildPoint(const Point& point, const std::shared_ptr<Control>& child) const;

    std::shared_ptr<Control> InnerFindChildAtPosition(
        const Point& position, 
        bool recursively) const;

    void NeedRelayout(const zaf::Rect& previous_rect);

    void SetInteractiveProperty(bool new_value, bool& property_value, void(Control::*notification)());

    bool HandleClickOnMouseDown();
    bool HandleClickOnMouseUp();

    std::shared_ptr<internal::InspectorPort> GetInspectorPort() const;

private:
    std::weak_ptr<Window> window_;
    std::weak_ptr<Control> parent_;
    std::vector<std::shared_ptr<Control>> children_;

    std::weak_ptr<internal::ControlUpdateLock> update_lock_;
    std::unique_ptr<internal::ControlUpdateState> update_state_;

    bool is_cached_painting_enabled_{};
    BitmapRenderer cached_renderer_;
    zaf::Rect valid_cached_renderer_rect_;

    bool is_hovered_;
    bool is_capturing_mouse_;
    bool is_focused_;
    bool can_focused_;
    bool is_enabled_;
    bool is_visible_;
    bool is_selected_{};

    zaf::Rect rect_;
    Frame margin_;
    Frame border_;
    Frame padding_;

    bool should_raise_click_event_{};
    std::uint32_t last_mouse_down_time_{};

    PropertyMap property_map_;
};


class ControlClickInfo {
public:
    std::shared_ptr<Control> control;
};


class ControlDoubleClickInfo {
public: 
    std::shared_ptr<Control> control;
};


class ControlFocusChangeInfo {
public:
    std::shared_ptr<Control> control;
};


class ControlMouseEnterInfo {
public:
    std::shared_ptr<Control> control;
    std::shared_ptr<Control> entered_control;
};


class ControlMouseLeaveInfo {
public:
    std::shared_ptr<Control> control;
    std::shared_ptr<Control> leaved_control;
};


class ControlRectChangeInfo {
public:
    std::shared_ptr<Control> control;
    zaf::Rect previous_rect;
};

}