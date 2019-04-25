#pragma once

#include <Windows.h>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <zaf/base/event.h>
#include <zaf/control/anchor.h>
#include <zaf/control/color_picker.h>
#include <zaf/control/layout/layouter.h>
#include <zaf/graphic/renderer/bitmap_renderer.h>
#include <zaf/graphic/color.h>
#include <zaf/graphic/frame.h>
#include <zaf/graphic/rect.h>
#include <zaf/serialization/property_map.h>
#include <zaf/serialization/serializable_object.h>

namespace zaf {

class Canvas;
class CharMessage;
class KeyMessage;
class Message;
class MouseMessage;
class MouseWheelMessage;
class Renderer;
class Window;

/**
 Represents a control in a window.

 This is the base class of all controls.
 */
class Control : public SerializableObject, public std::enable_shared_from_this<Control> {
public:
    ZAF_DECLARE_TYPE_NAME();

public:
    /**
     Provides a convenient way for automatically calling BeginUpdate/EndUpdate
     of a control.

     The control's BeginUpdate is called when the instance is created, and
     the EndUpdate is called when the instance is destroyed.
     */
    class UpdateGuard {
    public:
        /**
         Construct the instance with specified control.

         The control's BeginUpdate method would be called.
         */
        explicit UpdateGuard(Control& control) : control_(control) { 
            control_.BeginUpdate();
        }

        /**
         Destruct the instance.

         The control's EndUpdate method would be called.
         */
        ~UpdateGuard() {
            control_.EndUpdate();
        }

        UpdateGuard(const UpdateGuard&) = delete;
        UpdateGuard& operator=(const UpdateGuard&) = delete;

    private:
        Control& control_;
    };

    /**
     Type of rect change event.
     */
    typedef Event<const std::shared_ptr<Control>&, const Rect&> RectChangeEvent;

    /**
     Type of focus change event.
     */
    typedef Event<const std::shared_ptr<Control>&> FocusChangeEvent;

public:
	Control();
	virtual ~Control();

	/**
	 Initialize the control.

	 Derived classes can override this method to do some initialization,
	 such as setting initial property values, and adding child controls.
	 The same method of base class must be called.
	 */
	virtual void Initialize() { }

    /**
     Begin updating the contol.

     This method increase the control's update count. The control would stop repainting 
     and relayouting while its update count is greater than zero. 

     The method EndUpdate must be called in order to decrease the update count. It is 
     recommended that using UpdateGuard to ensure that the callings of BeginUpdate/EndUpdate 
     are matched.
     */
    void BeginUpdate();

    /**
     End updating the control.

     This method decrease the control's udpate count, and if it reachs zero, the control would
     repaint and relayout immediately.
     */
    void EndUpdate();

	/**
	 Get the control's absolute rect which is related to the coordinate system of 
	 window's content rect.

	 If the control is not yet placed in a window, am empty rect is returned.
	 */
	Rect GetAbsoluteRect() const;

	/**
	 Get the control's rect which is related to the coordinate system of parent's
	 content rect.

	 The default rect is empty.
	 */
	const Rect& GetRect() const {
		return rect_;
	}

	/**
	 Set the control's rect.

	 See also GetRect.
	 */
	void SetRect(const Rect& rect);

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
		SetRect(Rect(position, GetRect().size));
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
		SetRect(Rect(GetRect().position, size));
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
    float GetMinimumWidth() const;

    /**
     Set the control's minimum width.
     */
    void SetMinimumWidth(float min_width);

    /**
     Get the control's maximum width.

     The default value is the maximum value of float.
     */
    float GetMaximumWidth() const;

    /**
     Set the control's maximum width.
     */
    void SetMaximumWidth(float max_width);

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
    float GetMinimumHeight() const;

    /**
     Set the control's minimum height.
     */
    void SetMinimumHeight(float min_height);

    /**
     Get the control's maximum height.

     The default value is the maximum value of float.
     */
    float GetMaximumHeight() const;

    /**
     Set the control's maximum height.
     */
    void SetMaximumHeight(float max_height);

    /**
     Get the control's anchor.

     The default value is Anchor::None.
     */
	Anchor GetAnchor() const;

    /**
     Set the control's anchor.
     */
	void SetAnchor(Anchor anchor);

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
	Rect GetContentRect() const;

    /**
     Get the control's content size.
     */
    Size GetContentSize() const {
        return GetContentRect().size;
    }

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
	Layouter GetLayouter() const;

    /**
     Set the layouter.

     Setting the layouter would causes the control to relayout immediately.
     */
	void SetLayouter(const Layouter& layouter);

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

    std::shared_ptr<Control> FindChildAtPosition(const Point& position) const;

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

     The default value is InvalidIndex.
     */
    std::size_t GetTabIndex() const;

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

    /**
     Get rect change event.

     This event is raised when the control's rect is changed.
     */
    RectChangeEvent::Proxy GetRectChangeEvent();

    /**
     Get focus change event.

     This event is raised when the control's focus is changed.
     */
    FocusChangeEvent::Proxy GetFocusChangeEvent();

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
    virtual bool AcceptKeyMessage(const KeyMessage& message) const {
        return false;
    }

protected:
    /**
     Paint the control.

     @param canvas
        The canvas used to paint.

     @param dirty_rect
        The rectangle area needed to be repainted in the control.

     Derived classes can override this method to paint the control.
     */
	virtual void Paint(Canvas& canvas, const Rect& dirty_rect);

    /**
     Require the control to repaint.
     */
	void NeedRepaint();

    /**
     Require the control to repaint specified rectangle area.
     */
	void NeedRepaintRect(const Rect& rect);

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
	virtual void Layout(const Rect& previous_rect);

    /**
     Require the control to relayout its children.
     */
	void NeedRelayout();

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

     This method is called when a WM_MOUSEMOVE message is received. Derived classes should 
     call the same method of base class if they don't process the notification.
     */
    virtual void MouseMove(const Point& position, const MouseMessage& message);

    /**
     Process the mouse enter notification.

     @param entered_control
         The control which mouse enters. It is the current control or its children.

     This method is called when the mouse has entered the control. Derived classes should 
     call the same method of base class if they don't process the notification.
     */
	virtual void MouseEnter(const std::shared_ptr<Control>& entered_control);

    /**
     Process the mouse leave notification.

     @param entered_control
         The control which mouse leaves. It is the current control or its children.

     This method is called when the mouse has left the control. Derived classes should 
     call the same method of base class if they don't process the notifiction.
     */
    virtual void MouseLeave(const std::shared_ptr<Control>& leaved_control);

    /**
     Process the mouse down notification.

     @param position
        The mouse position in the control's coordinate.

     @param message
        Information about the mouse down message (e.g. WM_LBUTTONDOWN).

     This method is called when a mouse button is pressed within the control. Derived classes 
     should call the same method of base class if they don't process the notifiction.
     */
    virtual void MouseDown(const Point& position, const MouseMessage& message);

    /**
     Process the mouse up notification.

     @param position
        The mouse position in the control's coordinate.

     @param message
        Information about the mouse up message (e.g. WM_LBUTTONUP).

     This method is called when a mouse button is released within the control. Derived classes 
     should call the same method of base class if they don't process the notifiction.
     */
    virtual void MouseUp(const Point& position, const MouseMessage& message);

    /**
     Process the mouse wheel notification.

     @param position
        The mouse position in the control's coordinate.

     @param message
        Information about the mouse wheel message (e.g. WM_MOUSEWHEEL).

     This method is called when the mouse wheel within the control. Derived classes
     should call the same method of base class if they don't process the notification.
     */
    virtual void MouseWheel(const Point& position, const MouseWheelMessage& message);

    /**
     Process the mouse capture notification.

     This method is called when the control captured the mouse. Derived classes
     should call the same method of base class if they don't process the notification.
     */
	virtual void MouseCapture();

    /**
     Process the mouse release notification.

     This method is called when the control release the mouse. Derived classes
     should call the same method of base class if they don't process the notification.
     */
	virtual void MouseRelease();

    /**
     Process the key down notification.

     This method is called when a key is pressed at mean while the control is focused. Derived 
     classes should call the same method of base class if they don't process the notification.
     */
    virtual bool KeyDown(const KeyMessage& message);

    /**
     Process the key up notification.

     This method is called when a key is released at mean while the control is focused. Derived 
     classes should call the same method of base class if they don't process the notification.
     */
    virtual bool KeyUp(const KeyMessage& message);

    /**
     Process the char input notification.

     This method is called when a char is inputed at mean while the control is focused. Derived 
     classes should call the same method of base class if they don't process the notification.
     */
    virtual bool CharInput(const CharMessage& message);

    /**
     Process the focus gain notification.

     This method is called when the control gained the focus. Derived classes should call the 
     same method of base class if they don't process the notification.
     */
	virtual void FocusGain();

    /**
     Process the focus lose notification.

     This method is called when the control lost the focus. Derived classes should call the 
     same method of base class if they don't process the notification.
     */
	virtual void FocusLose();

    /**
     Process the rect change notification.

     This method is called when the control change its rect. Derived classes must call the 
     same method of base class.

     If the size is changed, this method is called before calling Layout method. You don't 
     need to call NeedRelayout method in this method, and you should layout children in Layout
     method instead of this method.
     */
    virtual void RectChange(const Rect& previous_rect);

    /**
     Process the is visible change notification.

     This method is called when the control change its visibility. Derived classes must call
     the same method of base class.
     */
    virtual void IsVisibleChange();

    /**
     Process the is enabled change notification.

     This method is called when the control is enabled or disabled. Derived classes must call 
     the same method of base class.
     */
    virtual void IsEnabledChange();

    void SerializeToDataNode(DataNode& data_node) const override;
    bool DeserializeFromDataNode(const DataNode& data_node) override;

    virtual void DeserializeProperty(const std::wstring& name, const DataNode& data_node);

private:
	friend class Window;

	void SetWindow(const std::shared_ptr<Window>& window) {
		window_ = window;
	}

	void IsHoveredChanged(bool is_hovered);
	void IsFocusedChanged(bool is_focused);
	void IsCapturingMouseChanged(bool is_capturing_mouse);

	void RouteHoverMessage(const Point& position);
    void RouteMessage(const Point& position, const MouseMessage& message);
    void InterpretMessage(const Point& position, const MouseMessage& message);

private:
    /**
     The entry point to repaint the control.

     @param canvas
         Canvas used to paint the content of control.

     @param dirty_rect
         The rect in control's coordinate needed to repaint.
     */
    void Repaint(Canvas& canvas, const Rect& dirty_rect);

    void RepaintControl(Canvas& canvas, const Rect& dirty_rect);
    void RecalculateCachedPaintingRect(const Rect& repaint_rect);

	void SetParent(const std::shared_ptr<Control>& parent);

	/**
	 Called when a child's rect has changed.
	 */
	void ChildRectChanged(const std::shared_ptr<Control>& child, const Rect& previous_rect);

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

    void NeedRelayout(const Rect& previous_rect);

    bool IsUpdating() const {
        return update_count_ > 0;
    }

    void SetInteractiveProperty(bool new_value, bool& property_value, void(Control::*notification)());

    void SerializeProperties(DataNode& data_node) const;
    void SerializeChildren(DataNode& data_node) const;
    void DeserializeChildren(const DataNode& data_node);

	Control(const Control&) = delete;
	Control& operator=(const Control&) = delete;

private:
	std::weak_ptr<Window> window_;
	std::weak_ptr<Control> parent_;
	std::vector<std::shared_ptr<Control>> children_;

    bool is_cached_painting_enabled_{};
    BitmapRenderer cached_renderer_;
    Rect valid_cached_renderer_rect_;

    std::size_t update_count_;
    Rect need_repaint_rect_after_updating_;
    bool need_relayout_after_updating_;

	bool is_hovered_;
	bool is_capturing_mouse_;
	bool is_focused_;
	bool can_focused_;
	bool is_enabled_;
	bool is_visible_;

	Rect rect_;
    Frame border_;
    Frame padding_;

	PropertyMap property_map_;
};

}