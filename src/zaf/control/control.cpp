#include <zaf/control/control.h>
#include <algorithm>
#include <zaf/application.h>
#include <zaf/base/log.h>
#include <zaf/control/control_painter.h>
#include <zaf/graphic/canvas.h>
#include <zaf/internal/anchor_layouter.h>
#include <zaf/window/window.h>

namespace zaf {

static Point ToChildPoint(const Point& point, const std::shared_ptr<Control>& child);

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


void Control::Initialize() {

	painter_ = ControlPainter::GetInstance();
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


bool Control::IsParentOf(const std::shared_ptr<Control>& child) const {
	return child->GetParent().get() == this;
}


bool Control::IsAncestorOf(const std::shared_ptr<Control>& child) const {

	auto ancestor = child->GetParent();
	while (ancestor != nullptr) {

		if (ancestor.get() == this) {
			return true;
		}

		ancestor = ancestor->GetParent();
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

	Rect previous_rect = GetRect();
	rect_ = rect;

	Layout(previous_rect);

	//The focused control need to be notified while its absolute position changed, 
	//so that it can relayout its elements, if needed.
	auto window = GetWindow();
	if (window != nullptr) {
		auto focused_control = window->GetFocusedControl();
		if (focused_control != nullptr) {
			if (IsAncestorOf(focused_control)) {
				focused_control->NeedRelayout();
			}
		}
	}

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

	Rect paintable_rect = canvas.GetAbsolutePaintableRect();

	for (const auto& child : children_) {

		const Rect& child_rect = child->GetRect();

		Rect child_dirty_rect = dirty_rect;
		child_dirty_rect.Intersect(child_rect);

		if (child_dirty_rect.IsEmpty()) {
			//No need to repaint child if it doesn't locate in dirty rect
			continue;
		}

		const Rect& child_canvas_absolute_rect = child->GetAbsoluteRect();
		canvas.SetRects(
			child_canvas_absolute_rect, 
			Rect::Intersect(paintable_rect, child_canvas_absolute_rect)
		);

		child_dirty_rect.position.x -= child_rect.position.x;
		child_dirty_rect.position.y -= child_rect.position.y;
		child->Repaint(canvas, child_dirty_rect);
	}
}


void Control::Paint(Canvas& canvas, const Rect& dirty_rect) {
	painter_->Paint(canvas, dirty_rect, *this);
}


void Control::NeedRelayout() {
	Layout(GetRect());
}


void Control::Layout(const Rect& previous_rect) {

	internal::AnchorLayouter layouter;
	layouter.Layout(*this, previous_rect);
}


void Control::RouteHoverMessage(const Point& position) {

	auto child = FindChildAtPosition(position);
	if (child != nullptr) {
		child->RouteHoverMessage(ToChildPoint(position, child));
	}
	else {

		auto window = GetWindow();
		if (window != nullptr) {

			if (IsEnabled()) {
				window->SetHoveredControl(shared_from_this());
			}
			else {
				window->SetHoveredControl(nullptr);
			}
		}
	}
}


void Control::RouteMessage(const Point& position, UINT message, WPARAM wParam, LPARAM lParam) {

	auto child = FindChildAtPosition(position);
	if (child != nullptr) {
		child->RouteMessage(ToChildPoint(position, child), message, wParam, lParam);
	}
	else {

		if (IsEnabled()) {
			InterpretMessage(position, message, wParam, lParam);
		}
	}
}


std::shared_ptr<Control> Control::FindChildAtPosition(const Point& position) const {

	for (auto iterator = children_.rbegin(); iterator != children_.rend(); ++iterator) {

		const auto& child = *iterator;

		if (! child->IsVisible()) {
			continue;
		}

		const Rect& child_rect = child->GetRect();
		if (child_rect.Contain(position)) {
			return child;
		}
	}

	return nullptr;
}


void Control::InterpretMessage(const Point& position, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message) {
	case WM_MOUSEMOVE:
		MouseMove(position, wParam, lParam);
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


void Control::ChangeMouseCursor(WPARAM wParam, LPARAM lParam, bool& is_changed) {

	auto parent = GetParent();
	if (parent != nullptr) {
		parent->ChangeMouseCursor(wParam, lParam, is_changed);
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


void Control::CharInput(WPARAM wParam, LPARAM lParam) {

	auto parent = GetParent(); 
	if (parent != nullptr) {
		parent->CharInput(wParam, lParam);
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

	ZAFLOG() << "IsFocusedChanged: " << is_focused;

	is_focused_ = is_focused;

	if (is_focused_) {
		FocusGain();
	}
	else {
		FocusLose();
	}
}


void Control::FocusGain() {

}


void Control::FocusLose() {

}


void Control::SetPainter(const std::shared_ptr<Painter>& painter) {

	if (painter == nullptr) {
		painter_ = ControlPainter::GetInstance();
	}
	else {
		painter_ = painter;
	}

	NeedRepaint();
}


const Color Control::GetDefaultBackgroundColor() {
	static const Color default_background_color = Color::FromRGB(0xCFD6E5);
	return default_background_color;
}


const Color Control::GetDefaultForegroundColor() {
	return Color::Black;
}


const Color Control::GetBackgroundColor() const {

	if (background_color_.HasValue()) {
		return background_color_.GetValue();
	}

	auto parent = GetParent();
	if (parent != nullptr) {
		return parent->GetBackgroundColor();
	}

	return GetDefaultBackgroundColor();
}


const Color Control::GetForegroundColor() const {

	if (foreground_color_.HasValue()) {
		return foreground_color_.GetValue();
	}

	return GetDefaultForegroundColor();
}


static Point ToChildPoint(const Point& point, const std::shared_ptr<Control>& child) {

	Point point_in_child = point;
	point_in_child.x -= child->GetRect().position.x;
	point_in_child.y -= child->GetRect().position.y;
	return point_in_child;
}

}