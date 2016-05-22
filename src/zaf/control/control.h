#pragma once

#include <Windows.h>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <zaf/base/property_map.h>
#include <zaf/control/anchor.h>
#include <zaf/control/color_picker.h>
#include <zaf/control/layout/layouter.h>
#include <zaf/graphic/color.h>
#include <zaf/graphic/rect.h>

namespace zaf {

class Canvas;
class KeyMessage;
class Message;
class MouseMessage;
class MouseWheelMessage;
class Window;

class Control : public std::enable_shared_from_this<Control> {
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
	 Get the control's absolute rect which is related to the coordinate system of 
	 window's content rect.

	 If the control is not yet placed in a window, am empty rect is returned.
	 */
	const Rect GetAbsoluteRect() const;

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
	 Get the control's height.
	 */
	float GetHeight() const {
		return rect_.size.height;
	}

	Anchor GetAnchor() const;
	void SetAnchor(Anchor anchor);

	/**
	 Get the control's border width.

	 The border is not included in control's content rect.
	 The default bordr width is 0.
	 */
	float GetBorderWidth() const {
		return border_width_;
	}

	/**
	 Set the control's border width.

	 See also GetBorderWidth.
	 */
	void SetBorderWidth(float width);

	/**
	 Get the control's content rect, related to its coordinate system.
	 */
	const Rect GetContentRect() const;

    /**
     Get the control's content size.
     */
    const Size GetContentSize() const {
        return GetContentRect().size;
    }

	const Color GetBackgroundColor() const {
		return GetBackgroundColorPicker()(*this);
	}
	const ColorPicker GetBackgroundColorPicker() const;
	void SetBackgroundColorPicker(const ColorPicker& color_picker);

	const Color GetBorderColor() const {
		return GetBorderColorPicker()(*this);
	}
	const ColorPicker GetBorderColorPicker() const;
	void SetBorderColorPicker(const ColorPicker& color_picker);

	const Layouter GetLayouter() const;
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
	 Remoe specified child from the control.
	 */
	void RemoveChild(const std::shared_ptr<Control>& child);

    const std::shared_ptr<Control> FindChildAtPosition(const Point& position) const;

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
	const std::shared_ptr<Control> GetParent() const {
		return parent_.lock();
	}

	/**
	 Get the control's name.

	 The name is used to identify a child in control. Multiple 
	 children may have the same name. The default name is empty.
	 */
	const std::wstring GetName() const;

	/**
	 Set the control's name.

	 See also GetName.
	 */
	void SetName(const std::wstring& name);

	/**
	 Get the window where the control locates.

	 Return nullptr if the controls does not locates in any window.
	 */
	const std::shared_ptr<Window> GetWindow() const;

	/**
	 Get a value indicating that whether the control is visible.

	 The default value is true.
	 */
	bool IsVisible() const {
		return is_visible_;
	}

	/**
	 Set a value indicating that whether the control is visible.

	 See also IsVisible. 
	 */
	void SetIsVisible(bool is_visible);

	/**
	 Get a value indicating that whether the control is enabled.

	 The default value is true.
	 */
	bool IsEnabled() const {
		return is_enabled_;
	}

	/**
	 Set a value indicating that whether the control is enabled.

	 See also IsEnabled.
	 */
	void SetIsEnabled(bool is_enabled);

	/**
	 Get a value indicating that whether the control is hovered.
	 */
	bool IsHovered() const {
		return is_hovered_;
	}

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
	 Get a value indicating that whether the control is focused.
	 */
	bool IsFocused() const {
		return is_focused_;
	}

	/**
	 Set a value indicating that whether the control is focused.
	 */
	void SetIsFocused(bool is_focused);

	const Point GetMousePosition() const;

protected:
	virtual void Repaint(Canvas& canvas, const Rect& dirty_rect);
	virtual void Paint(Canvas& canvas, const Rect& dirty_rect);

	void NeedRepaint();
	void NeedRepaintRect(const Rect& rect);

	virtual void Layout(const Rect& previous_rect);
	void NeedRelayout();

	PropertyMap& GetPropertyMap() {
		return property_map_;
	}

	const PropertyMap& GetPropertyMap() const {
		return property_map_;
	}

	bool IsCapturingMouse() const {
		return is_capturing_mouse_;
	}

	void NeedCaptureMouse(bool capture);

	virtual void ChangeMouseCursor(const Message& message, bool& is_changed);
    virtual void MouseMove(const Point& position, const MouseMessage& message);
	virtual void MouseEnter();
	virtual void MouseLeave();
    virtual void MouseDown(const Point& position, const MouseMessage& message);
    virtual void MouseUp(const Point& position, const MouseMessage& message);
    virtual void MouseWheel(const Point& position, const MouseWheelMessage& message);
	virtual void MouseCapture();
	virtual void MouseRelease();
    virtual void KeyDown(const KeyMessage& message);
    virtual void KeyUp(const KeyMessage& message);
    virtual void CharInput(const KeyMessage& message);
	virtual void FocusGain();
	virtual void FocusLose();

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
	void SetParent(const std::shared_ptr<Control>& parent) {
		parent_ = parent;
	}

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

	Control(const Control&) = delete;
	Control& operator=(const Control&) = delete;

private:
	std::weak_ptr<Window> window_;
	std::weak_ptr<Control> parent_;
	std::vector<std::shared_ptr<Control>> children_;
	bool is_hovered_;
	bool is_capturing_mouse_;
	bool is_focused_;
	bool can_focused_;
	bool is_enabled_;
	bool is_visible_;

	Rect rect_;
	float border_width_;

	PropertyMap property_map_;
};

}