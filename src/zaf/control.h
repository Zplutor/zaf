#pragma once

#include <Windows.h>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include "color.h"
#include "enum.h"
#include "rect.h"

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
	bool HasChild(const std::shared_ptr<Control>& child) const;

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

	const Size& GetSize() const {
		return rect_.size;
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

	const std::wstring& GetText() const {
		return text_;
	}

	void SetText(const std::wstring& text) {
		text_ = text;
		NeedRepaint();
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
	virtual void Paint(Canvas& canvas, const Rect& dirty_rect);

	void NeedRelayout();
	virtual void Layout(const Rect& previous_rect);

	void NeedCaptureMouse(bool capture);
	bool IsCapturingMouse() const {
		return is_capturing_mouse_;
	}

	virtual void MouseMove(const Point& position, WPARAM wParam, LPARAM lParam);
	virtual void MouseEnter();
	virtual void MouseLeave();
	virtual void MouseDown(const Point& position, MouseButton button, WPARAM wParam, LPARAM lParam);
	virtual void MouseUp(const Point& position, MouseButton button, WPARAM wParam, LPARAM lParam);
	virtual void MouseWheel(const Point& position, bool is_horizontal, int distance, WPARAM wParam, LPARAM lParam);
	virtual void MouseCapture();
	virtual void MouseRelease();

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

	virtual void Repaint(Canvas& canvas, const Rect& dirty_rect);
	virtual void RouteMessage(const Point& position, UINT message, WPARAM wParam, LPARAM lParam);

private:
	void CheckInitialized();

	void SetParent(const std::shared_ptr<Control>& parent) {
		parent_ = parent;
	}

	/**
	 Called when a child's rect has changed.
	 */
	void ChildRectChanged(const std::shared_ptr<Control>& child, const Rect& previous_rect);

	void InterpretMessage(const Point& position, UINT message, WPARAM wParam, LPARAM lParam);
	void InterpretMouseMove(const Point& position, WPARAM wParam, LPARAM lParam);

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

	Rect rect_;
	std::set<Anchor> anchors_;
	std::wstring text_;
};

}