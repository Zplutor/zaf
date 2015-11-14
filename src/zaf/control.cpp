#include "control.h"
#include <algorithm>
#include "application.h"
#include "canvas.h"
#include "window.h"
#include "internal/log.h"
#include <zaf/internal/anchor_layouter.h>
#include <zaf/internal/theme/painter.h>
#include <zaf/internal/theme/theme.h>

namespace zaf {


Control::Control() : 
	is_initialized_(false),
	is_hovered_(false), 
	is_capturing_mouse_(false),
	is_focused_(false),
	can_focused_(false),
	is_enabled_(true),
	is_visible_(true) {

}


Control::~Control() {

}


void Control::CheckInitialized() {

	if (is_initialized_) {
		return;
	}

	//This flag must be set before calling Initialize,
	//because AddChild may be called in Initialize.
	is_initialized_ = true;
	Initialize();

	for (auto& each_child : children_) {
		each_child->CheckInitialized();
	}
}


const Point Control::GetMousePosition() const {

	auto window = GetWindow();
	if (window == nullptr) {
		return Point();
	}
	
	Point mouse_position = window->GetMousePosition();
	Point absolute_position = GetAbsoluteRect().position;
	mouse_position.x -= absolute_position.x;
	mouse_position.y -= absolute_position.y;
	return mouse_position;
}


const std::shared_ptr<Window> Control::GetWindow() const {

	auto parent = GetParent();
	if (parent == nullptr) {
		return window_.lock();
	}

	return parent->GetWindow();
}


void Control::AddChild(const std::shared_ptr<Control>& child) {

	auto previous_parent = child->GetParent();

	if (previous_parent.get() == this) {
		//Already added
		return;
	}

	if (previous_parent != nullptr) {
		//Remove child from previous parent
		previous_parent->RemoveChild(child);
	}

	//Initialize the child if current control has initialized.
	if (is_initialized_) {
		child->CheckInitialized();
	}

	child->SetParent(this->shared_from_this());
	children_.push_back(child);
	
	NeedRepaintRect(child->GetRect());
}


void Control::RemoveChild(const std::shared_ptr<Control>& child) {

	auto current_parent = child->GetParent();
	if (current_parent.get() != this) {
		//Not child
		return;
	}

	child->SetParent(nullptr);
	children_.erase(std::remove(children_.begin(), children_.end(), child));

	NeedRepaintRect(child->GetRect()); 
}


bool Control::HasChild(const std::shared_ptr<Control>& child) const {

	for (const auto& each_child : children_) {
		if (each_child == child) {
			return true;
		}
	}

	return false;
}


const Rect Control::GetAbsoluteRect() const {

	auto parent = GetParent();
	if (parent == nullptr) {
		return rect_;
	}

	Rect parent_absolute_rect = parent->GetAbsoluteRect();
	return Rect(
		parent_absolute_rect.position.x + rect_.position.x,
		parent_absolute_rect.position.y + rect_.position.y,
		rect_.size.width,
		rect_.size.height);
}


void Control::SetRect(const Rect& rect) {

	Rect previous_rect = rect_;
	rect_ = rect;

	Layout(previous_rect);

	auto parent = GetParent();
	if (parent != nullptr) {
		parent->ChildRectChanged(this->shared_from_this(), previous_rect);
	}
}


void Control::ChildRectChanged(const std::shared_ptr<Control>& child, const Rect& previous_rect) {

	const Rect& new_rect = child->GetRect();

	if (new_rect.HasIntersection(previous_rect)) {
		NeedRepaintRect(Rect::Union(new_rect, previous_rect));
	}
	else {
		NeedRepaintRect(new_rect);
		NeedRepaintRect(previous_rect);
	}
}


void Control::SetIsEnabled(bool is_enabled) {

	if (is_enabled_ == is_enabled) {
		return;
	}

	if (IsHovered()) {
		auto window = GetWindow();
		if (window != nullptr) {
			window->SetHoveredControl(nullptr);
		}
	}

	SetIsFocused(false);

	is_enabled_ = is_enabled;
	NeedRepaint();

	for (auto& each_child : children_) {
		each_child->SetIsEnabled(is_enabled);
	}
}


void Control::SetIsVisible(bool is_visible) {

	if (is_visible_ = is_visible) {
		return;
	}

	if (IsHovered()) {
		auto window = GetWindow();
		if (window != nullptr) {
			window->SetHoveredControl(nullptr);
		}
	}

	SetIsFocused(false);

	is_visible_ = is_visible;
	NeedRepaint();

	for (auto& each_child : children_) {
		each_child->SetIsVisible(is_visible);
	}
}


void Control::NeedRepaint() {
	NeedRepaintRect(Rect(Point(), rect_.size));
}


void Control::NeedRepaintRect(const Rect& rect) {

	Rect bound_rect(Point(), rect_.size);
	Rect repaint_rect = Rect::Intersect(bound_rect, rect);
	if (repaint_rect.IsEmpty()) {
		return;
	}

	auto window = window_.lock();
	if (window != nullptr) {
		window->NeedRepaintRect(repaint_rect);
		return;
	}

	auto parent = GetParent();
	if (parent == nullptr) {
		return;
	}

	Rect repaint_rect_in_parent(ToParentPoint(repaint_rect.position), repaint_rect.size);
	parent->NeedRepaintRect(repaint_rect_in_parent);
}


void Control::Repaint(Canvas& canvas, const Rect& dirty_rect) {

	if (! IsVisible()) {
		return;
	}

	canvas.BeginPaint();
	Paint(canvas, dirty_rect);
	canvas.EndPaint();

	for (const auto& child : children_) {

		const Rect& child_rect = child->GetRect();

		Rect child_dirty_rect = dirty_rect;
		child_dirty_rect.Intersect(child_rect);

		if (child_dirty_rect.IsEmpty()) {
			//No need to repaint child if it doesn't locate in dirty rect
			continue;
		}

		const Rect& child_canvas_absolute_rect = child->GetAbsoluteRect();
		Canvas child_canvas(
			canvas.GetRenderer(),
			child_canvas_absolute_rect, 
			Rect::Intersect(canvas.GetAbsoluteDirtyRect(), child_canvas_absolute_rect)
		);

		child_dirty_rect.position.x -= child_rect.position.x;
		child_dirty_rect.position.y -= child_rect.position.y;
		child->Repaint(child_canvas, child_dirty_rect);
	}
}


void Control::Paint(Canvas& canvas, const Rect& dirty_rect) {
	internal::theme::GetTheme()->GetDefaultPainter()->Paint(canvas, dirty_rect, *this);
}


void Control::NeedRelayout() {
	Layout(GetRect());
}


void Control::Layout(const Rect& previous_rect) {

	internal::AnchorLayouter layouter;
	layouter.Layout(*this, previous_rect);
}


void Control::RouteMessage(const Point& position, UINT message, WPARAM wParam, LPARAM lParam) {

	bool is_found = false;

	for (auto iterator = children_.rbegin(); iterator != children_.rend(); ++iterator) {

		auto child = *iterator;

		if (! child->IsEnabled() || ! child->IsVisible()) {
			continue;
		}

		const Rect& child_rect = child->GetRect();
		if (child_rect.Contain(position)) {

			//Translate the position in child's coordinate system
			Point position_in_child = position;
			position_in_child.x -= child_rect.position.x;
			position_in_child.y -= child_rect.position.y;
			child->RouteMessage(position_in_child, message, wParam, lParam);

			is_found = true;
			break;
		}
	}

	if (! is_found) {
		InterpretMessage(position, message, wParam, lParam);
	}
}


void Control::InterpretMessage(const Point& position, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message) {

	case WM_MOUSEMOVE:
		InterpretMouseMove(position, wParam, lParam);
		break;

	case WM_LBUTTONDOWN:
		MouseDown(position, MouseButton::Left, wParam, lParam);
		break;
	case WM_MBUTTONDOWN:
		MouseDown(position, MouseButton::Middle, wParam, lParam);
		break;
	case WM_RBUTTONDOWN:
		MouseDown(position, MouseButton::Right, wParam, lParam);
		break;

	case WM_LBUTTONUP:
		MouseUp(position, MouseButton::Left, wParam, lParam);
		break;
	case WM_MBUTTONUP:
		MouseUp(position, MouseButton::Middle, wParam, lParam);
		break;
	case WM_RBUTTONUP:
		MouseUp(position, MouseButton::Right, wParam, lParam);
		break;

	case WM_MOUSEWHEEL:
		MouseWheel(position, false, static_cast<short>(HIWORD(wParam)), wParam, lParam);
		break;
	case WM_MOUSEHWHEEL:
		MouseWheel(position, true, static_cast<short>(HIWORD(wParam)), wParam, lParam);
		break;

	default:
		break;
	}
}


void Control::InterpretMouseMove(const Point& position, WPARAM wParam, LPARAM lParam) {
	
	if (! is_hovered_) {
		auto window = GetWindow();
		if (window != nullptr) {
			window->SetHoveredControl(this->shared_from_this());
		}
	}

	MouseMove(position, wParam, lParam);
}


void Control::IsHoveredChanged(bool is_hovered) {

	if (is_hovered_ == is_hovered) {
		return;
	}

	is_hovered_ = is_hovered;

	if (is_hovered_) {
		MouseEnter();
	}
	else {
		MouseLeave();
	}

	NeedRepaint();
}


void Control::NeedCaptureMouse(bool capture) {

	auto window = GetWindow();
	if (window != nullptr) {

		auto shared_this = shared_from_this();

		if (capture && ! IsHovered()) {
			window->SetHoveredControl(shared_this);
		}

		window->SetCaptureMouseControl(shared_this, ! capture);
	}
}


void Control::IsCapturingMouseChanged(bool is_capturing_mouse) {

	is_capturing_mouse_ = is_capturing_mouse;

	if (is_capturing_mouse_) {
		MouseCapture();
	}
	else {
		MouseRelease();
	}
}


void Control::MouseMove(const Point& position, WPARAM wParam, LPARAM lParam) {

	auto parent = GetParent();
	if (parent != nullptr) {
		parent->MouseMove(ToParentPoint(position), wParam, lParam);
	}
}


void Control::MouseEnter() {

}


void Control::MouseLeave() {

}


void Control::MouseDown(const Point& position, MouseButton button, WPARAM wParam, LPARAM lParam) {

	auto parent = GetParent();
	if (parent != nullptr) {
		parent->MouseDown(ToParentPoint(position), button, wParam, lParam);
	}
}


void Control::MouseUp(const Point& position, MouseButton button, WPARAM wParam, LPARAM lParam) {

	auto parent = GetParent();
	if (parent != nullptr) {
		parent->MouseUp(ToParentPoint(position), button, wParam, lParam);
	}
}


void Control::MouseWheel(const Point& position, bool is_horizontal, int distance, WPARAM wParam, LPARAM lParam) {

	auto parent = GetParent();
	if (parent != nullptr) {
		parent->MouseWheel(position, is_horizontal, distance, wParam, lParam);
	}
}


void Control::MouseCapture() {

}


void Control::MouseRelease() {

}


void Control::KeyDown(WPARAM wParam, LPARAM lParam) {

	auto parent = GetParent();
	if (parent != nullptr) {
		parent->KeyDown(wParam, lParam);
	}
}


void Control::KeyUp(WPARAM wParam, LPARAM lParam) {

	auto parent = GetParent();
	if (parent != nullptr) {
		parent->KeyUp(wParam, lParam);
	}
}


void Control::SetIsFocused(bool is_focused) {

	if (! CanFocused()) {
		return;
	}

	if (IsFocused() == is_focused) {
		return;
	}

	auto window = GetWindow();
	if (window != nullptr) {
		window->SetFocusedControl(is_focused ? shared_from_this() : nullptr);
	}
}


void Control::IsFocusedChanged(bool is_focused) {

	if (is_focused_ == is_focused) {
		return;
	}

	LOG() << "IsFocusedChanged: " << is_focused;

	is_focused_ = is_focused;

	if (is_focused_) {
		FocusGain();
	}
	else {
		FocusLose();
	}

	NeedRepaint();
}


void Control::FocusGain() {

}


void Control::FocusLose() {

}

}