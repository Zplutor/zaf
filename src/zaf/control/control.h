#pragma once

#include <Windows.h>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <zaf/base/auto_reset_value.h>
#include <zaf/control/anchor.h>
#include <zaf/control/color_picker.h>
#include <zaf/control/event/double_click_info.h>
#include <zaf/control/event/focus_event_info.h>
#include <zaf/control/event/keyboard_event_info.h>
#include <zaf/control/event/mouse_event_info.h>
#include <zaf/control/event/parent_changed_info.h>
#include <zaf/control/event/rect_changed_info.h>
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
    zaf::Rect AbsoluteRect() const;

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

    void SetFixedWidth(float width) {
        SetMaxWidth(width);
        SetMinWidth(width);
    }

    void SetFixedHeight(float height) {
        SetMaxHeight(height);
        SetMinHeight(height);
    }

    void SetFixedSize(const zaf::Size& size) {
        SetFixedWidth(size.width);
        SetFixedHeight(size.height);
    }

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
    Color BackgroundColor() const {
        return BackgroundColorPicker()(*this);
    }

    /**
     Get the color picker of background.
     */
    ColorPicker BackgroundColorPicker() const;

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
    Color BorderColor() const {
        return BorderColorPicker()(*this);
    }

    /**
     Get the color picker of border color.
     */
    ColorPicker BorderColorPicker() const;

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
    bool IsParentOf(const std::shared_ptr<const Control>& child) const;

    /**
     Determinte whether the control is the ancestor of specified control.
     */
    bool IsAncestorOf(const std::shared_ptr<const Control>& child) const;

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
     Get a value indicating that whether the control itself is under mouse cursor.
     */
    bool IsMouseOver() const {
        return is_mouse_over_;
    }

    /**
     Get a value indicating that whether the control or any of its children is under mouse cursor.
     */
    bool ContainMouse() const;

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

    bool ContainFocus() const;

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
     Release the mouse.
     */
    void ReleaseMouse();

    /**
     The the mouse position in control's coordinate.
     */
    const Point GetMousePosition() const;

    float GetDPI() const;

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
     Requires the control to repaint.
     */
    void NeedRepaint();

    /**
     Requires the control to repaint specified rectangle area.
     */
    void NeedRepaintRect(const zaf::Rect& rect);

    /**
     Translate a point in current control's coordinate system to which in parent's coordinate 
     system.
     */
    Point TranslateToParentPoint(const Point& point) const;

    /**
     Get rect change event.

     This event is raised when the control's rect is changed.
     */
    Observable<RectChangedInfo> RectChangedEvent();

    Observable<FocusGainedInfo> FocusGainedEvent();
    Observable<FocusLostInfo> FocusLostEvent();

    Observable<MouseEnterInfo> MouseEnterEvent();
    Observable<MouseLeaveInfo> MouseLeaveEvent();
    Observable<MouseHoverInfo> MouseHoverEvent();

    Observable<MouseDownInfo> MouseDownEvent();
    Observable<MouseUpInfo> MouseUpEvent();
    Observable<DoubleClickInfo> DoubleClickEvent();

    Observable<KeyDownInfo> KeyDownEvent();
    Observable<KeyUpInfo> KeyUpEvent();
    Observable<CharInputInfo> CharInputEvent();

    Observable<ParentChangedInfo> ParentChangedEvent();

protected:
    void InvokeInitialize() override;
    void InvokeParse() override;

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

    virtual void OnMouseMove(const MouseMoveInfo& event_info);
    virtual void OnMouseEnter(const MouseEnterInfo& event_info);
    virtual void OnMouseLeave(const MouseLeaveInfo& event_info);
    virtual void OnMouseHover(const MouseHoverInfo& event_info);

    virtual void OnMouseDown(const MouseDownInfo& event_info);
    virtual void OnMouseUp(const MouseUpInfo& event_info);

    virtual void OnDoubleClick(const DoubleClickInfo& event_info);
    virtual void OnMouseWheel(const MouseWheelInfo& event_info);

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

    virtual void OnKeyDown(const KeyDownInfo& event_info);
    virtual void OnKeyUp(const KeyUpInfo& event_info);
    virtual void OnCharInput(const CharInputInfo& event_info);

    virtual void OnFocusGained(const FocusGainedInfo& event_info);
    virtual void OnFocusLost(const FocusLostInfo& event_info);

    virtual void OnRectChanged(const RectChangedInfo& event_info);

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

    virtual void OnDPIChanged();

    virtual void OnParentChanged(const ParentChangedInfo& event_info);

private:
    friend class Caret;
    friend class Window;

    void SetWindow(const std::shared_ptr<zaf::Window>& window) {
        window_ = window;
    }

    void SetIsMouseOverByWindow(bool is_mouse_over);
    void SetIsFocusedByWindow(bool is_focused);
    void IsCapturingMouseChanged(bool is_capturing_mouse);
    void FindMouseOverControl(const Point& position, const MouseMessage& message);
    std::shared_ptr<Control> FindEnabledControlAtPosition(const Point& position);

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

    void ApplyAutoSizeOnRectChanged(zaf::Size& new_size);
    void AutoResizeToPreferredSize();
    zaf::Size CalculatePreferredSizeForAutoSize(const zaf::Size& control_size) const;

    /**
     Called when a child's rect has changed.
     */
    void OnChildRectChanged(const std::shared_ptr<Control>& child, const zaf::Rect& previous_rect);

    /**
     Tanslate a point to which in specified child's coordinate system.
     */
    Point ToChildPoint(const Point& point, const std::shared_ptr<Control>& child) const;

    std::shared_ptr<Control> InnerFindChildAtPosition(
        const Point& position, 
        bool recursively) const;

    void NeedRelayout(const zaf::Rect& previous_rect);

    void SetInteractiveProperty(bool new_value, bool& property_value, void(Control::*notification)());

    bool HandleDoubleClickOnMouseDown(const Point& position);

    std::shared_ptr<internal::InspectorPort> GetInspectorPort() const;

private:
    std::weak_ptr<zaf::Window> window_;
    std::weak_ptr<Control> parent_;
    std::vector<std::shared_ptr<Control>> children_;

    std::weak_ptr<internal::ControlUpdateLock> update_lock_;
    std::unique_ptr<internal::ControlUpdateState> update_state_;

    bool is_cached_painting_enabled_{};
    BitmapRenderer cached_renderer_;
    zaf::Rect valid_cached_renderer_rect_;

    bool auto_width_{};
    bool auto_height_{};
    AutoResetValue<bool> is_auto_resizing_{ false };
    AutoResetValue<bool> is_layouting_{ false };

    bool is_mouse_over_;
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

    std::uint32_t last_mouse_down_time_{};
    Point last_mouse_down_position_;

    PropertyMap property_map_;
};

}