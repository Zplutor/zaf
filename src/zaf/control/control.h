#pragma once

#include <Windows.h>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <zaf/enum.h>
#include <zaf/base/nullable.h>
#include <zaf/base/rect.h>
#include <zaf/base/variant.h>
#include <zaf/control/anchor.h>
#include <zaf/graphic/color.h>
#include <zaf/graphic/text/paragraph_alignment.h>
#include <zaf/graphic/text/text_alignment.h>
#include <zaf/graphic/text/word_wrapping.h>

namespace zaf {

class Canvas;
class Layouter;
class TextProperties;
class Window;

class Control : public std::enable_shared_from_this<Control> {
public:
	class PaintComponent {
	public:
		static const int Background = 0;
		static const int Foreground = 1;
		static const int Border = 2;
		static const int Custom = 100;

	private:
		PaintComponent();
	};

	class PaintState {
	public:
		static const int Normal = 0;
		static const int Hovered = 1;
		static const int Focused = 2;
		static const int Disabled = 3;
		static const int Custom = 100;

	private:
		PaintState();
	};

public:
	Control();
	virtual ~Control();

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
	 Get the color of specified component while in specified state.
	 */
	virtual const Color GetColor(int paint_component, int paint_state) const;

	/**
	 Set the color of specified component while in specified state.
	 */
	virtual void SetColor(int paint_component, int paint_state, const Color& color);

	/**
	 Get the control's text value.
	 */
	virtual const std::wstring GetText() const;

	/**
	 Set the control's text value.
	 */
	virtual void SetText(const std::wstring& text);

	/**
	 Get text properties.

	 If text properties is not set, the font family name and the font size would use
	 the same value as parent's. Other properties would be the default value.
	 */
	virtual const TextProperties GetTextProperties() const;

	/**
	 Set text properties.
	 */
	virtual void SetTextProperties(const TextProperties& text_properties);


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
	class PropertyMap {
	public:
		template<typename PropertyType>
		const PropertyType* TryGetProperty(const std::wstring& property_name) const {

			auto iterator = properties_.find(property_name);
			if (iterator == properties_.end()) {
				return nullptr;
			}

			return iterator->second.TryCast<PropertyType>();
		}

		template<typename PropertyType>
		const PropertyType GetProperty(
			const std::wstring& property_name, 
			const std::function<const PropertyType()>& get_default_property
		) const {

			auto property = TryGetProperty<PropertyType>(property_name);
			if (property == nullptr) {
				return get_default_property();
			}

			return *property;
		}

		template<typename PropertyType>
		void SetProperty(const std::wstring& property_name, const PropertyType& property_value) {
			properties_[property_name] = Variant(property_value);
		}

	private:
		std::map<std::wstring, Variant> properties_;
	};

protected:
	/**
	 Initialize the control.

	 Derived classes can override this method to do some initialization, 
	 such as adding child controls. The same method of base class must be called.
	 */
	virtual void Initialize();

	virtual void Repaint(Canvas& canvas, const Rect& dirty_rect);
	virtual void Paint(Canvas& canvas, const Rect& dirty_rect);

	/**
	 Get the current paint state.

	 This method is called while painting the control, to get its state.
	 */
	virtual int GetPaintState() const;

	/**
	 Paint text in specified rect.

	 The text is not painted by default. Derived classes should call this method
	 within its Paint method to display text.
	 */
	void PaintText(Canvas& canvas, const Rect& dirty_rect, const Rect& text_rect);

	void NeedRepaint();
	void NeedRepaintRect(const Rect& rect);

	virtual void Layout(const Rect& previous_rect);
	void NeedRelayout();

	const PropertyMap& GetPropertyMap() const {
		return property_map_;
	}

	PropertyMap& GetPropertyMap() {
		return property_map_;
	}

	bool IsCapturingMouse() const {
		return is_capturing_mouse_;
	}

	void NeedCaptureMouse(bool capture);

	virtual void ChangeMouseCursor(WPARAM wParam, LPARAM lParam, bool& is_changed);
	virtual void MouseMove(const Point& position, WPARAM wParam, LPARAM lParam);
	virtual void MouseEnter();
	virtual void MouseLeave();
	virtual void MouseDown(const Point& position, MouseButton button, WPARAM wParam, LPARAM lParam);
	virtual void MouseUp(const Point& position, MouseButton button, WPARAM wParam, LPARAM lParam);
	virtual void MouseWheel(const Point& position, bool is_horizontal, int distance, WPARAM wParam, LPARAM lParam);
	virtual void MouseCapture();
	virtual void MouseRelease();
	virtual void KeyDown(WPARAM wParam, LPARAM lParam);
	virtual void KeyUp(WPARAM wParam, LPARAM lParam);
	virtual void CharInput(WPARAM wParam, LPARAM lParam);
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
	void RouteMessage(const Point& position, UINT message, WPARAM wParam, LPARAM lParam);

private:
	void CheckInitialized();

	void LayoutChild(const std::shared_ptr<Control>& child, const Rect& previous_rect);

	const Color GetBackgroundColor(int paint_state) const;
	const Color GetForegroundColor(int paint_state) const;
	const Color GetBorderColor(int paint_state) const;
	const Color GetPropertyColor(
		const std::wstring& color_property_name,
		const std::function<const Color()>& get_default_color
	) const;

	void SetParent(const std::shared_ptr<Control>& parent) {
		parent_ = parent;
	}

	std::shared_ptr<Control> FindChildAtPosition(const Point& position) const;

	/**
	 Called when a child's rect has changed.
	 */
	void ChildRectChanged(const std::shared_ptr<Control>& child, const Rect& previous_rect);

	void InterpretMessage(const Point& position, UINT message, WPARAM wParam, LPARAM lParam);

	/**
	 Translate a point to which in parent's coordinate system.
	 */
	Point ToParentPoint(const Point& point) {
		return Point(point.x + rect_.position.x, point.y + rect_.position.y);
	}

	Control(const Control&) = delete;
	Control& operator=(const Control&) = delete;

private:
	bool is_initialized_;
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