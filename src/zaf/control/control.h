#pragma once

#include <Windows.h>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <zaf/enum.h>
#include <zaf/base/nullable.h>
#include <zaf/base/rect.h>
#include <zaf/base/variant.h>
#include <zaf/graphic/color.h>

namespace zaf {

class Canvas;
class Layouter;
class Window;

class Control : public std::enable_shared_from_this<Control> {
public:
	enum class Anchor {
		Left,
		Top,
		Right,
		Bottom,
	};

public:
	/**
	 Get the default background color.
	 */
	static const Color GetDefaultBackgroundColor();

	/**
	 Get the default foreground color.
	 */
	static const Color GetDefaultForegroundColor();

public:
	Control();
	virtual ~Control();

	const Point GetMousePosition() const;

	const std::shared_ptr<Window> GetWindow() const;

	const std::shared_ptr<Control> GetParent() const {
		return parent_.lock();
	}

	const std::vector<std::shared_ptr<Control>>& GetChildren() const {
		return children_;
	}

	void AddChild(const std::shared_ptr<Control>& child);
	void RemoveChild(const std::shared_ptr<Control>& child);
	bool IsParentOf(const std::shared_ptr<Control>& child) const;
	bool IsAncestorOf(const std::shared_ptr<Control>& child) const;

	bool IsHovered() const {
		return is_hovered_;
	}

	bool IsFocused() const {
		return is_focused_;
	}

	void SetIsFocused(bool is_focused);

	bool CanFocused() const {
		return can_focused_;
	}

	void SetCanFocused(bool can_focused) {
		can_focused_ = can_focused;
	}

	bool IsEnabled() const {
		return is_enabled_;
	}

	void SetIsEnabled(bool is_enabled);

	bool IsVisible() const {
		return is_visible_;
	}

	void SetIsVisible(bool is_visible);

	/**
	 Get the absolute rect in window.
	 */
	const Rect GetAbsoluteRect() const;

	const Rect& GetRect() const {
		return rect_;
	}

	void SetRect(const Rect& rect);

	const Point& GetPosition() const {
		return rect_.position;
	}

	void SetPosition(const Point& position) {
		SetRect(Rect(position, GetRect().size));
	}

	const Size& GetSize() const {
		return rect_.size;
	}

	void SetSize(const Size& size) {
		SetRect(Rect(GetRect().position, size));
	}

	float GetWidth() const {
		return rect_.size.width;
	}

	float GetHeight() const {
		return rect_.size.height;
	}

	float GetBorderWidth() const {
		return border_width_;
	}

	void SetBorderWidth(float width) {
		border_width_ = width;
		NeedRepaint();
	}

	const std::set<Anchor>& GetAnchors() const {
		return anchors_;
	}

	void AddAnchor(Anchor anchor) {
		anchors_.insert(anchor);
	}

	void RemoveAnchor(Anchor anchor) {
		anchors_.erase(anchor);
	}

	const Color GetBackgroundColor() const;
	void SetBackgroundColor(const Color& color);

	const Color GetHoveredBackgroundColor() const;
	void SetHoveredBackgroundColor(const Color& color);

	const Color GetFocusedBackgroundColor() const;
	void SetFocusedBackgroundColor(const Color& color);

	const Color GetDisabledBackgroundColor() const;
	void SetDisabledBackgroundColor(const Color& color);

	const Color GetForegroundColor() const;
	void SetForegroundColor(const Color& color);

	const Color GetHoveredForegroundColor() const;
	void SetHoveredForegroundColor(const Color& color);

	const Color GetFocusedForegroundColor() const;
	void SetFocusedForegroundColor(const Color& color);

	const Color GetDisabledForegroundColor() const;
	void SetDisabledForegroundColor(const Color& color);

	const Color GetBorderColor() const;
	void SetBorderColor(const Color& color);

	const Color GetHoveredBorderColor() const;
	void SetHoveredBorderColor(const Color& color);

	const Color GetFocusedBorderColor() const;
	void SetFocusedBorderColor(const Color& color);

	const Color GetDisabledBorderColor() const;
	void SetDisabledBorderColor(const Color& color);

	const std::wstring GetName() const;
	void SetName(const std::wstring& name);

	/**
	 Get the text value.
	 */
	virtual const std::wstring GetText() const;

	/**
	 Set the text value.
	 */
	virtual void SetText(const std::wstring& text);

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
		const PropertyType GetProperty(const std::wstring& property_name) const {

			auto property = TryGetProperty<PropertyType>(property_name);
			if (property == nullptr) {
				return PropertyType();
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

	enum class PaintComponent {
		Background,
		Border,
		Foreground,
	};

protected:
	/**
	 Initialize the control.

	 Derived classes can override this method to do some initialization, 
	 such as adding child controls. The same method of base class must be called.
	 */
	virtual void Initialize();

	void NeedRepaint();
	void NeedRepaintRect(const Rect& rect);
	virtual void Repaint(Canvas& canvas, const Rect& dirty_rect);
	virtual void Paint(Canvas& canvas, const Rect& dirty_rect);

	/**
	 Paint text in specified rect.

	 The text is not painted by default. Derived classes should call this method
	 within its Paint method to display text.
	 */
	void PaintText(Canvas& canvas, const Rect& dirty_rect, const Rect& text_rect);

	/**
	 Get the current color of specified paint component.

	 This method is called while painting particular component, to get its color.

	 The default implementation returns color according to the control's state, 
	 including normal, hovered, focused and disabled. Derived classes can override
	 this method if they need a different color in some case.
	 */
	virtual const Color GetPaintColor(PaintComponent component) const;

	const PropertyMap& GetPropertyMap() const {
		return property_map_;
	}

	PropertyMap& GetPropertyMap() {
		return property_map_;
	}

	void NeedRelayout();
	virtual void Layout(const Rect& previous_rect);

	void NeedCaptureMouse(bool capture);
	bool IsCapturingMouse() const {
		return is_capturing_mouse_;
	}

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

	const Color GetSpecialColor(
		const std::wstring& property_name,
		const Color(Control::*get_default_color)() const
	) const {

		auto color = property_map_.TryGetProperty<Color>(property_name);
		if (color != nullptr) {
			return *color;
		}

		return (this->*get_default_color)();
	}

	void SetColor(const std::wstring& property_name, const Color& new_color) {
		property_map_.SetProperty(property_name, new_color);
		NeedRepaint();
	}

private:
	friend class Window;

	void SetWindow(const std::shared_ptr<Window>& window) {
		window_ = window;
	}

	void IsHoveredChanged(bool is_hovered);
	void IsCapturingMouseChanged(bool is_capturing_mouse);
	void IsFocusedChanged(bool is_focused);

	void RouteHoverMessage(const Point& position);
	void RouteMessage(const Point& position, UINT message, WPARAM wParam, LPARAM lParam);

private:
	void CheckInitialized();

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
	std::set<Anchor> anchors_;

	PropertyMap property_map_;
};

}