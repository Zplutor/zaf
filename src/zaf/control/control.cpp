#include <zaf/control/control.h>
#include <algorithm>
#include <zaf/base/assert.h>
#include <zaf/control/layout/anchor_layouter.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/font/font.h>
#include <zaf/graphic/geometry/path_geometry.h>
#include <zaf/graphic/geometry/rectangle_geometry.h>
#include <zaf/window/message/message.h>
#include <zaf/window/message/mouse_message.h>
#include <zaf/window/window.h>

namespace zaf {

static Point ToChildPoint(const Point& point, const std::shared_ptr<Control>& child);

static const wchar_t* const kAnchorPropertyName = L"Anchor";
static const wchar_t* const kBackgroundColorPickerPropertyName = L"BackgroundColorPicker";
static const wchar_t* const kBorderColorPickerPropertyName = L"BorderColorPicker";
static const wchar_t* const kLayouterPropertyName = L"Layouter";
static const wchar_t* const kNamePropertyName = L"Name";

Control::Control() : 
    update_count_(0),
	is_hovered_(false), 
	is_capturing_mouse_(false),
	is_focused_(false),
	can_focused_(false),
	is_enabled_(true),
	is_visible_(true),
	border_width_(0) {

}


Control::~Control() {

}


void Control::BeginUpdate() {
    ++update_count_;
}


void Control::EndUpdate() {

    if (update_count_ == 0) {
        ZAF_FAIL();
        return;
    }

    --update_count_;

    if (update_count_ == 0) {
        NeedRelayout();
        NeedRepaint();
    }
}


void Control::Repaint(Canvas& canvas, const Rect& dirty_rect) {

	if (! IsVisible()) {
		return;
	}

	canvas.BeginPaint();
	Paint(canvas, dirty_rect);
	canvas.EndPaint();

	float border_width = GetBorderWidth();
	Rect content_rect = GetContentRect();

	const Rect& absolute_rect = canvas.GetAbsoluteRect();
	Rect absolute_content_rect = content_rect;
	absolute_content_rect.position.x += absolute_rect.position.x;
	absolute_content_rect.position.y += absolute_rect.position.y;

	Rect absolute_paintable_rect = canvas.GetAbsolutePaintableRect();
	absolute_paintable_rect.Intersect(absolute_content_rect);
	
	for (const auto& child : children_) {

		const Rect& child_rect = child->GetRect();

		Rect child_dirty_rect = Rect::Intersect(content_rect, dirty_rect);
		child_dirty_rect.position.x -= border_width;
		child_dirty_rect.position.y -= border_width;
		child_dirty_rect.Intersect(child_rect);

		if (child_dirty_rect.IsEmpty()) {
			//No need to repaint child if it doesn't locate in dirty rect
			continue;
		}

		const Rect& child_canvas_absolute_rect = child->GetAbsoluteRect();
		canvas.SetRects(
			child_canvas_absolute_rect,
			Rect::Intersect(absolute_paintable_rect, child_canvas_absolute_rect)
		);

		child_dirty_rect.position.x -= child_rect.position.x;
		child_dirty_rect.position.y -= child_rect.position.y;
		child->Repaint(canvas, child_dirty_rect);
	}
}


void Control::Paint(Canvas& canvas, const Rect& dirty_rect) {

	Canvas::StateGuard state_guard(canvas);

    Rect border_rect(Point(), GetSize());
  
    Rect background_rect = border_rect;
    background_rect.Inflate(-GetBorderWidth());
    canvas.SetBrushWithColor(GetBackgroundColor());
    canvas.DrawRectangle(background_rect);

    auto border_geometry = canvas.CreateRectangleGeometry(border_rect);
    auto background_geometry = canvas.CreateRectangleGeometry(background_rect);
    
    auto path_geometry = canvas.CreatePathGeometry();
    auto sink = path_geometry->Open();
    Geometry::Combine(border_geometry, background_geometry, Geometry::CombineMode::Exclude, sink);
    sink->Close();

	canvas.SetBrushWithColor(GetBorderColor());
    canvas.DrawGeometry(path_geometry);
}


void Control::NeedRepaint() {
	NeedRepaintRect(Rect(Point(), rect_.size));
}


void Control::NeedRepaintRect(const Rect& rect) {

    if (IsUpdating()) {
        return;
    }

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

    Point position_in_parent = ToParentPoint(repaint_rect.position);
    float parent_border_width = parent->GetBorderWidth();
    position_in_parent.x += parent_border_width;
    position_in_parent.y += parent_border_width;

	Rect repaint_rect_in_parent(position_in_parent, repaint_rect.size);
    repaint_rect_in_parent.Intersect(parent->GetContentRect());
	parent->NeedRepaintRect(repaint_rect_in_parent);
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


void Control::Layout(const Rect& previous_rect) {

	auto layouter = GetLayouter();
	if (layouter != nullptr) {
		layouter(shared_from_this(), previous_rect, GetChildren());
	}
}


void Control::NeedRelayout() {

    if (! IsUpdating()) {
	    Layout(GetRect());
    }
}


const Rect Control::GetAbsoluteRect() const {

	auto window = GetWindow();
	if (window == nullptr) {
		return Rect();
	}

	auto parent = GetParent();
	if (parent == nullptr) {
		return Rect();
	}

	Rect parent_absolute_rect = parent->GetAbsoluteRect();
	float parent_border_width = parent->GetBorderWidth();
	return Rect(
		parent_absolute_rect.position.x + parent_border_width + rect_.position.x,
		parent_absolute_rect.position.y + parent_border_width + rect_.position.y,
		rect_.size.width,
		rect_.size.height
	);
}


void Control::SetRect(const Rect& rect) {

	Rect previous_rect = GetRect();
	rect_ = rect;

    //Layout children if size is changed.
    if (rect_.size != previous_rect.size) {
	    Layout(previous_rect);
    }

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


Anchor Control::GetAnchor() const {

	auto anchor = GetPropertyMap().TryGetProperty<Anchor>(kAnchorPropertyName);
	if (anchor != nullptr) {
		return *anchor;
	}
	return Anchor::None;
}


void Control::SetAnchor(Anchor anchor) {
	GetPropertyMap().SetProperty(kAnchorPropertyName, anchor);
}


void Control::SetBorderWidth(float width) {

	border_width_ = width;
	NeedRelayout();
	NeedRepaint();
}


const Rect Control::GetContentRect() const {

	Rect content_rect = Rect(Point(), GetSize());
	content_rect.Inflate(-GetBorderWidth());
	return content_rect;
}


const ColorPicker Control::GetBackgroundColorPicker() const {

	auto color_picker = GetPropertyMap().TryGetProperty<ColorPicker>(kBackgroundColorPickerPropertyName);
	if ((color_picker != nullptr) && (*color_picker != nullptr)) {
		return *color_picker;
	}

	return [](const Control&) {
		return Color::Transparent;
	};
}


void Control::SetBackgroundColorPicker(const ColorPicker& color_picker) {

	GetPropertyMap().SetProperty(kBackgroundColorPickerPropertyName, color_picker);
	NeedRepaint();
}


const ColorPicker Control::GetBorderColorPicker() const {

	auto color_picker = GetPropertyMap().TryGetProperty<ColorPicker>(kBorderColorPickerPropertyName);
	if ((color_picker != nullptr) && (*color_picker != nullptr)) {
		return *color_picker;
	}

	return [](const Control& control) {
		return control.GetBackgroundColorPicker()(control);
	};
}


void Control::SetBorderColorPicker(const ColorPicker& color_picker) {

	GetPropertyMap().SetProperty(kBorderColorPickerPropertyName, color_picker);
	NeedRepaint();
}


const Layouter Control::GetLayouter() const {

	auto layouter = GetPropertyMap().TryGetProperty<Layouter>(kLayouterPropertyName);
	if (layouter != nullptr) {
		return *layouter;
	}
	else {
		return GetAnchorLayouter();
	}
}

void Control::SetLayouter(const Layouter& layouter) {
	GetPropertyMap().SetProperty(kLayouterPropertyName, layouter);
    NeedRelayout();
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

	child->SetParent(shared_from_this());
	children_.push_back(child);

    NeedRelayout();
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

    NeedRelayout();
	NeedRepaintRect(child->GetRect());
}


const std::shared_ptr<Control> Control::FindChildAtPosition(const Point& position) const {

    for (auto iterator = children_.rbegin(); iterator != children_.rend(); ++iterator) {

        const auto& child = *iterator;

        if (! child->IsVisible()) {
            continue;
        }

        Rect child_rect = child->GetRect();
        child_rect.Intersect(GetContentRect());

        if (child_rect.Contain(position)) {
            return child;
        }
    }

    return nullptr;
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


const std::wstring Control::GetName() const {

	auto name = GetPropertyMap().TryGetProperty<std::wstring>(kNamePropertyName);
	if (name != nullptr) {
		return *name;
	}
	else {
		return std::wstring();
	}
}


void Control::SetName(const std::wstring& name) {
	GetPropertyMap().SetProperty(kNamePropertyName, name);
}


const std::shared_ptr<Window> Control::GetWindow() const {

	auto parent = GetParent();
	if (parent == nullptr) {
		return window_.lock();
	}

	return parent->GetWindow();
}


void Control::SetIsVisible(bool is_visible) {

	if (is_visible_ == is_visible) {
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


void Control::SetIsFocused(bool is_focused) {

	if (!CanFocused()) {
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

	is_focused_ = is_focused;

	if (is_focused_) {
		FocusGain();
	}
	else {
		FocusLose();
	}
}


void Control::NeedCaptureMouse(bool capture) {

	auto window = GetWindow();
	if (window != nullptr) {

		auto shared_this = shared_from_this();

		if (capture && !IsHovered()) {
			window->SetHoveredControl(shared_this);
		}

		window->SetCaptureMouseControl(shared_this, !capture);
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


void Control::RouteMessage(const Point& position, const MouseMessage& message) {

	auto child = FindChildAtPosition(position);
	if (child != nullptr) {
        child->RouteMessage(ToChildPoint(position, child), message);
	}
	else {

		if (IsEnabled()) {
            InterpretMessage(position, message);
		}
	}
}


void Control::InterpretMessage(const Point& position, const MouseMessage& message) {

	switch (message.id) {
	    case WM_MOUSEMOVE:
		    MouseMove(position, message);
		    break;

	    case WM_LBUTTONDOWN:
	    case WM_MBUTTONDOWN:
	    case WM_RBUTTONDOWN:
            MouseDown(position, message);
		    break;

	    case WM_LBUTTONUP:
	    case WM_MBUTTONUP:
	    case WM_RBUTTONUP:
            MouseUp(position, message);
		    break;

	    case WM_MOUSEWHEEL:
	    case WM_MOUSEHWHEEL:
            MouseWheel(position, dynamic_cast<const MouseWheelMessage&>(message));
		    break;

	    default:
		    break;
	}
}


void Control::ChangeMouseCursor(const Message& message, bool& is_changed) {

	auto parent = GetParent();
	if (parent != nullptr) {
		parent->ChangeMouseCursor(message, is_changed);
	}
}


void Control::MouseMove(const Point& position, const MouseMessage& message) {

	auto parent = GetParent();
	if (parent != nullptr) {
        parent->MouseMove(ToParentPoint(message.position), message);
	}
}


void Control::MouseEnter() {

}


void Control::MouseLeave() {

}


void Control::MouseDown(const Point& position, const MouseMessage& message) {

	auto parent = GetParent();
	if (parent != nullptr) {
        parent->MouseDown(ToParentPoint(position), message);
	}
}


void Control::MouseUp(const Point& position, const MouseMessage& message) {

	auto parent = GetParent();
	if (parent != nullptr) {
        parent->MouseUp(ToParentPoint(position), message);
	}
}


void Control::MouseWheel(const Point& position, const MouseWheelMessage& message) {

	auto parent = GetParent();
	if (parent != nullptr) {
		parent->MouseWheel(ToParentPoint(position), message);
	}
}


void Control::MouseCapture() {

}


void Control::MouseRelease() {

}


void Control::KeyDown(const KeyMessage& message) {

	auto parent = GetParent();
	if (parent != nullptr) {
		parent->KeyDown(message);
	}
}


void Control::KeyUp(const KeyMessage& message) {

	auto parent = GetParent();
	if (parent != nullptr) {
		parent->KeyUp(message);
	}
}


void Control::CharInput(const KeyMessage& message) {

	auto parent = GetParent(); 
	if (parent != nullptr) {
		parent->CharInput(message);
	}
}


void Control::FocusGain() {

}


void Control::FocusLose() {

}


static Point ToChildPoint(const Point& point, const std::shared_ptr<Control>& child) {

	Point point_in_child = point;
	point_in_child.x -= child->GetRect().position.x;
	point_in_child.y -= child->GetRect().position.y;
	return point_in_child;
}


}