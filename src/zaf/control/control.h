#pragma once

#include <Windows.h>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <zaf/enum.h>
#include <zaf/base/nullable.h>
#include <zaf/base/rect.h>
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
	static const Color GetDefaultBackgroundColor() {
		return Color::White;
	}

	/**
	 Get the default foreground color.
	 */
	static const Color GetDefaultForegroundColor() {
		return Color::Black;
	}

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
	
	void SetBackgroundColor(const Color& color) {
		SetColor(background_color_, color);
	}

	const Color GetHoveredBackgroundColor() const {
		return GetSpecialBackgroundColor(hovered_background_color_);
	}

	void SetHoveredBackgroundColor(const Color& color) {
		SetColor(hovered_background_color_, color);
	}

	const Color GetFocusedBackgroundColor(const Color& color) const {
		return GetSpecialBackgroundColor(focused_background_color_);
	}

	void SetFocusedBackgroundColor(const Color& color) {
		SetColor(focused_background_color_, color);
	}

	const Color GetDisabledBackgroundColor() const {
		return GetSpecialBackgroundColor(disabled_background_color_);
	}

	void SetDisabledBackgroundColor(const Color& color) {
		SetColor(disabled_background_color_, color);
	}

	const Color GetForegroundColor() const;

	void SetForegroundColor(const Color& color) {
		SetColor(foreground_color_, color);
	}

	const Color GetHoveredForegroundColor() const {
		return GetSpecialForegroundColor(hovered_foreground_color_);
	}

	void SetHoveredForegroundColor(const Color& color) {
		SetColor(hovered_foreground_color_, color);
	}

	const Color GetFocusedForegroundColor() const {
		return GetSpecialForegroundColor(focused_foreground_color_);
	}

	void SetFocusedForegroundColor(const Color& color) {
		SetColor(focused_foreground_color_, color);
	}

	const Color GetDisabledForegroundColor() const {
		return GetSpecialForegroundColor(disabled_foreground_color_);
	}

	void SetDisabledForegroundColor(const Color& color) {
		SetColor(disabled_foreground_color_, color);
	}

	const Color GetBorderColor() const {
		if (border_color_.HasValue()) {
			return border_color_.GetValue();
		}
		return GetBackgroundColor();
	}

	void SetBorderColor(const Color& color) {
		SetColor(border_color_, color);
	}

	const Color GetHoveredBorderColor() const {
		return GetSpecialBorderColor(hovered_border_color_);
	}

	void SetHoveredBorderColor(const Color& color) {
		SetColor(hovered_border_color_, color);
	}

	const Color GetFocusedBorderColor() const {
		return GetSpecialBorderColor(focused_border_color_);
	}

	void SetFocusedBorderColor(const Color& color) {
		SetColor(focused_border_color_, color);
	}

	const Color GetDisabledBorderColor() const {
		return GetSpecialBorderColor(disabled_border_color_);
	}

	void SetDisabledBorderColor(const Color& color) {
		SetColor(disabled_border_color_, color);
	}

protected:
	/**
	 Initialize the control.

	 Derived classes can override this method to do some initialization, 
	 such as adding child controls. The same method of base class must be called.
	 */
	virtual void Initialize() { }

	void NeedRepaint();
	void NeedRepaintRect(const Rect& rect);
	virtual void Repaint(Canvas& canvas, const Rect& dirty_rect);
	virtual void Paint(Canvas& canvas, const Rect& dirty_rect);

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

	const Color GetSpecialBackgroundColor(const Nullable<Color>& color) const {
		if (color.HasValue()) {
			return color.GetValue();
		}
		return GetBackgroundColor();
	}

	const Color GetSpecialForegroundColor(const Nullable<Color>& color) const {
		if (color.HasValue()) {
			return color.GetValue();
		}
		return GetForegroundColor();
	}

	const Color GetSpecialBorderColor(const Nullable<Color>& color) const {
		if (color.HasValue()) {
			return color.GetValue();
		}
		return GetBorderColor();
	}

	void SetColor(Nullable<Color>& color, const Color& new_color) {
		color = new_color;
		NeedRepaint();
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
	std::set<Anchor> anchors_;

	Nullable<Color> background_color_;
	Nullable<Color> hovered_background_color_;
	Nullable<Color> focused_background_color_;
	Nullable<Color> disabled_background_color_;
	
	Nullable<Color> foreground_color_;
	Nullable<Color> hovered_foreground_color_;
	Nullable<Color> focused_foreground_color_;
	Nullable<Color> disabled_foreground_color_;

	Nullable<Color> border_color_;
	Nullable<Color> hovered_border_color_;
	Nullable<Color> focused_border_color_;
	Nullable<Color> disabled_border_color_;
};

}