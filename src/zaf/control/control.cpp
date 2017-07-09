#include <zaf/control/control.h>
#include <algorithm>
#include <zaf/base/assert.h>
#include <zaf/base/event_utility.h>
#include <zaf/control/layout/anchor_layouter.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/font/font.h>
#include <zaf/graphic/geometry/path_geometry.h>
#include <zaf/graphic/geometry/rectangle_geometry.h>
#include <zaf/graphic/resource_factory.h>
#include <zaf/internal/theme.h>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/properties.h>
#include <zaf/serialization/serializable_type.h>
#include <zaf/serialization/serialization_manager.h>
#include <zaf/window/message/message.h>
#include <zaf/window/message/mouse_message.h>
#include <zaf/window/window.h>

#undef max
#undef min

namespace zaf {
namespace {

const wchar_t* const kAnchorPropertyName = L"Anchor";
const wchar_t* const kBackgroundColorPickerPropertyName = L"BackgroundColorPicker";
const wchar_t* const kBorderColorPickerPropertyName = L"BorderColorPicker";
const wchar_t* const kFocusChangeEventPropertyName = L"FocusChangeEvent";
const wchar_t* const kLayouterPropertyName = L"Layouter";

const bool DefaultCanFocused = false;
const bool DefaultIsEnabled = true;
const bool DefaultIsVisible = true;

}

Control::Control() : 
    update_count_(0),
    need_relayout_after_updating_(false),
    need_repaint_after_updating_(false),
	is_hovered_(false), 
	is_capturing_mouse_(false),
	is_focused_(false),
    can_focused_(DefaultCanFocused),
    is_enabled_(DefaultIsEnabled),
    is_visible_(DefaultIsVisible) {

}


Control::~Control() {

}


void Control::BeginUpdate() {
    need_relayout_after_updating_ = false;
    need_repaint_after_updating_ = false;
    ++update_count_;
}


void Control::EndUpdate() {

    if (update_count_ == 0) {
        ZAF_FAIL();
        return;
    }

    --update_count_;

    if (update_count_ == 0) {

        if (need_relayout_after_updating_) {
            NeedRelayout();
        }

        if (need_repaint_after_updating_) {
            NeedRepaint();
        }
    }
}


void Control::Repaint(Canvas& canvas, const Rect& dirty_rect) {

	if (! IsVisible()) {
		return;
	}

	canvas.BeginPaint();
	Paint(canvas, dirty_rect);
	canvas.EndPaint();

    const auto& border = GetBorder();
    const auto& padding = GetPadding();
	Rect content_rect = GetContentRect();

    //Translate content rect to absolute content rect.
	const Rect& canvas_absolute_rect = canvas.GetAbsoluteRect();
	Rect absolute_content_rect = content_rect;
    absolute_content_rect.position.x += canvas_absolute_rect.position.x;
    absolute_content_rect.position.y += canvas_absolute_rect.position.y;

    //Intersect paintable rect to current control's content rect.
	Rect absolute_paintable_rect = canvas.GetAbsolutePaintableRect();
	absolute_paintable_rect.Intersect(absolute_content_rect);
	
	for (const auto& child : children_) {

		const Rect& child_rect = child->GetRect();

        //Intersect dirty rect with current control's content rect and
        //child's rect.
		Rect child_dirty_rect = Rect::Intersect(content_rect, dirty_rect);
        child_dirty_rect.position.x -= border.left + padding.left;
        child_dirty_rect.position.y -= border.top + padding.top;
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

        //Translate dirty rect to which in child's coordinate.
		child_dirty_rect.position.x -= child_rect.position.x;
		child_dirty_rect.position.y -= child_rect.position.y;
		child->Repaint(canvas, child_dirty_rect);
	}
}


void Control::Paint(Canvas& canvas, const Rect& dirty_rect) {

	Canvas::StateGuard state_guard(canvas);

    Rect border_rect;
    border_rect.size = canvas.MakeClearEdgeForFill(GetRect()).size;
  
    Rect background_rect = border_rect;
    background_rect.Deflate(GetBorder());
    //The with and height muse be greater than 0.
    if (background_rect.size.width < 0) {
        background_rect.size.width = 0;
    }
    if (background_rect.size.height < 0) {
        background_rect.size.height = 0;
    }
    canvas.SetBrushWithColor(GetBackgroundColor());
    canvas.DrawRectangle(background_rect);

    auto resource_factory = GetResourceFactory();
    auto border_geometry = resource_factory->CreateRectangleGeometry(border_rect);
    auto background_geometry = resource_factory->CreateRectangleGeometry(background_rect);
    
    auto path_geometry = resource_factory->CreatePathGeometry();
    auto sink = path_geometry.Open();
    Geometry::Combine(border_geometry, background_geometry, Geometry::CombineMode::Exclude, sink);
    sink.Close();

	canvas.SetBrushWithColor(GetBorderColor());
    canvas.DrawGeometry(path_geometry);
}


void Control::NeedRepaint() {
	NeedRepaintRect(Rect(Point(), rect_.size));
}


void Control::NeedRepaintRect(const Rect& rect) {

    if (IsUpdating()) {
        need_repaint_after_updating_ = true;
        return;
    }

    if ((rect.size.width == 0) || (rect.size.height == 0)) {
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
    const auto& parent_border = parent->GetBorder();
    const auto& parent_padding = parent->GetPadding();
    position_in_parent.x += parent_border.left + parent_padding.left;
    position_in_parent.y += parent_border.top + parent_padding.top;

	Rect repaint_rect_in_parent(position_in_parent, repaint_rect.size);
    repaint_rect_in_parent.Intersect(parent->GetContentRect());
	parent->NeedRepaintRect(repaint_rect_in_parent);
}


void Control::ReleaseRendererResources() {

    for (const auto& each_child : children_) {
        each_child->ReleaseRendererResources();
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


void Control::Layout(const Rect& previous_rect) {

	auto layouter = GetLayouter();
	if (layouter != nullptr) {
		layouter(shared_from_this(), previous_rect, GetChildren());
	}
}


void Control::NeedRelayout() {
    NeedRelayout(GetRect());
}


void Control::NeedRelayout(const Rect& previous_rect) {

    if (! IsUpdating()) {

        const auto& size = GetSize();
        if ((size.width != 0) && (size.height != 0)) {
            Layout(previous_rect);
        }
    }
    else {
        need_relayout_after_updating_ = true;
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
	const auto& parent_border = parent->GetBorder();
    const auto& parent_padding = parent->GetPadding();

	return Rect(
        parent_absolute_rect.position.x + parent_border.left + parent_padding.left + rect_.position.x,
        parent_absolute_rect.position.y + parent_border.top + parent_padding.top + rect_.position.y,
		rect_.size.width,
		rect_.size.height
	);
}


void Control::SetRect(const Rect& rect) {

    Rect previous_rect = GetRect();

    //Don't layout if rects are the same.
    if (rect == previous_rect) {
        return;
    }

    //Revise the size.
    float width = rect.size.width;
    width = std::max(width, GetMinimumWidth());
    width = std::min(width, GetMaximumWidth());

    float height = rect.size.height;
    height = std::max(height, GetMinimumHeight());
    height = std::min(height, GetMaximumHeight());

	rect_ = zaf::Rect(rect.position, zaf::Size(width, height));    

    //Layout children if size is changed.
    if (rect_.size != previous_rect.size) {
        NeedRelayout(previous_rect);
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


float Control::GetMinimumWidth() const {

    auto min_width = GetPropertyMap().TryGetProperty<float>(property::MinimumWidth);
    if (min_width != nullptr) {
        return *min_width;
    }
    else {
        return 0;
    }
}

void Control::SetMinimumWidth(float min_width) {

    GetPropertyMap().SetProperty(property::MinimumWidth, min_width);

    if (GetMaximumWidth() < min_width) {
        SetMaximumWidth(min_width);
    }

    if (GetWidth() < min_width) {
        SetWidth(min_width);
    }
}


float Control::GetMaximumWidth() const {

    auto max_width = GetPropertyMap().TryGetProperty<float>(property::MaximumWidth);
    if (max_width != nullptr) {
        return *max_width;
    }
    else {
        return std::numeric_limits<float>::max();
    }
}

void Control::SetMaximumWidth(float max_width) {

    GetPropertyMap().SetProperty(property::MaximumWidth, max_width);

    if (GetMinimumWidth() > max_width) {
        SetMinimumWidth(max_width);
    }

    if (GetWidth() > max_width) {
        SetWidth(max_width);
    }
}


float Control::GetMinimumHeight() const {

    auto min_height = GetPropertyMap().TryGetProperty<float>(property::MinimumHeight);
    if (min_height != nullptr) {
        return *min_height;
    }
    else {
        return 0;
    }
}

void Control::SetMinimumHeight(float min_height) {

    GetPropertyMap().SetProperty(property::MinimumHeight, min_height);

    if (GetMaximumHeight() < min_height) {
        SetMaximumHeight(min_height);
    }

    if (GetHeight() < min_height) {
        SetHeight(min_height);
    }
}


float Control::GetMaximumHeight() const {

    auto max_height = GetPropertyMap().TryGetProperty<float>(property::MaximumHeight);
    if (max_height != nullptr) {
        return *max_height;
    }
    else {
        return std::numeric_limits<float>::max();
    }
}

void Control::SetMaximumHeight(float max_height) {

    GetPropertyMap().SetProperty(property::MaximumHeight, max_height);

    if (GetMinimumHeight() > max_height) {
        SetMinimumHeight(max_height);
    }

    if (GetHeight() > max_height) {
        SetHeight(max_height);
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


Rect Control::GetContentRect() const {

	Rect content_rect = Rect(Point(), GetSize());
	content_rect.Deflate(GetBorder());
    content_rect.Deflate(GetPadding());
	return content_rect;
}


const ColorPicker Control::GetBackgroundColorPicker() const {

	auto color_picker = GetPropertyMap().TryGetProperty<ColorPicker>(kBackgroundColorPickerPropertyName);
	if ((color_picker != nullptr) && (*color_picker != nullptr)) {
		return *color_picker;
	}
    else {
        return [](const Control&) {
            return Color::FromRGB(internal::ControlBackgroundColorRGB);
        };
    }
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
    else {
        return [](const Control&) {
            return Color::FromRGB(internal::ControlBackgroundColorRGB);
        };
    }
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


void Control::SetParent(const std::shared_ptr<Control>& parent) {

    auto need_release_renderer_resources = [this, &parent]() {

        auto previous_parent = parent_.lock();
        if (previous_parent == nullptr) {
            return false;
        }

        if (parent == nullptr) {
            return true;
        }

        auto previous_window = previous_parent->GetWindow();
        if (previous_window == nullptr) {
            return false;
        }

        auto new_window = parent->GetWindow();
        if (new_window == nullptr) {
            return true;
        }

        return previous_window != new_window;
    };

    if (need_release_renderer_resources()) {
        ReleaseRendererResources();
    }
    
    parent_ = parent;
}


void Control::AddChild(const std::shared_ptr<Control>& child) {

    if (child.get() == this) {
        ZAF_FAIL();
        return;
    }

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


void Control::AddChildren(const std::vector<std::shared_ptr<Control>>& children) {

    UpdateGuard update_guard(*this);
    for (const auto& each_child : children) {
        AddChild(each_child);
    }
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


void Control::RemoveAllChildren() {

    for (const auto& each_child : children_) {
        each_child->SetParent(nullptr);
    }

    children_.clear();
    NeedRepaint();
}


const std::shared_ptr<Control> Control::FindChildAtPosition(const Point& position) const {

    auto content_rect = GetContentRect();

    auto position_in_content = position;
    position_in_content.x -= content_rect.position.x;
    position_in_content.y -= content_rect.position.y;

    content_rect.position.x = 0;
    content_rect.position.y = 0;

    for (auto iterator = children_.rbegin(); iterator != children_.rend(); ++iterator) {

        const auto& child = *iterator;

        if (! child->IsVisible()) {
            continue;
        }

        Rect child_rect = child->GetRect();
        child_rect.Intersect(content_rect);

        if (child_rect.Contain(position_in_content)) {
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

	auto name = GetPropertyMap().TryGetProperty<std::wstring>(property::Name);
	if (name != nullptr) {
		return *name;
	}
	else {
		return std::wstring();
	}
}


void Control::SetName(const std::wstring& name) {
	GetPropertyMap().SetProperty(property::Name, name);
}


const std::shared_ptr<Window> Control::GetWindow() const {

	auto parent = GetParent();
	if (parent == nullptr) {
		return window_.lock();
	}

	return parent->GetWindow();
}


const Renderer Control::GetRenderer() const {

    auto window = GetWindow();
    if (window != nullptr) {
        return window->GetRenderer();
    }
    else {
        return Renderer();
    }
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
        each_child->SetIsEnabled(IsEnabled());
    }

    IsEnabledChange();
}


void Control::IsHoveredChanged(bool is_hovered) {

	if (is_hovered_ == is_hovered) {
		return;
	}

    auto point = GetMousePosition();
    
	is_hovered_ = is_hovered;

	if (is_hovered_) {
		MouseEnter(shared_from_this());
	}
	else {
        MouseLeave(shared_from_this());
	}
}


bool Control::IsHoveredIndirectly() const {

    auto window = GetWindow();
    if (window == nullptr) {
        return false;
    }

    const auto& hovered_control = window->GetHoveredControl();
    if (hovered_control == nullptr) {
        return false;
    }

    return IsAncestorOf(hovered_control);
}


bool Control::CanTabStop() const {

    auto can_tab_stop = GetPropertyMap().TryGetProperty<bool>(property::CanTabStop);
    if (can_tab_stop != nullptr) {
        return *can_tab_stop;
    }
    else {
        return true;
    }
}


void Control::SetCanTabStop(bool can_tab_stop) {

    GetPropertyMap().SetProperty(property::CanTabStop, can_tab_stop);
}


std::size_t Control::GetTabIndex() const {

    auto tab_index = GetPropertyMap().TryGetProperty<std::size_t>(property::TabIndex);
    if (tab_index != nullptr) {
        return *tab_index;
    }
    else {
        return InvalidIndex;
    }
}


void Control::SetTabIndex(std::size_t tab_index) {

    GetPropertyMap().SetProperty(property::TabIndex, tab_index);
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

    auto focus_change_event = TryGetEventFromPropertyMap<FocusChangeEvent>(GetPropertyMap(), kFocusChangeEventPropertyName);
    if (focus_change_event != nullptr) {
        focus_change_event->Trigger(shared_from_this());
    }
}


Control::FocusChangeEvent::Proxy Control::GetFocusChangeEvent() {
    return GetEventProxyFromPropertyMap<FocusChangeEvent>(GetPropertyMap(), kFocusChangeEventPropertyName);
}


void Control::CaptureMouse() {

    auto window = GetWindow();
    if (window != nullptr) {
        window->SetCaptureMouseControl(shared_from_this(), false);
    }
}


void Control::ReleaseMouse() {

    auto window = GetWindow();
    if (window != nullptr) {
        window->SetCaptureMouseControl(shared_from_this(), true);
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


const Point Control::ToChildPoint(const Point& point, const std::shared_ptr<Control>& child) const {

    const auto& border = GetBorder();
    const auto& padding = GetPadding();
    const auto& child_position = child->GetPosition();

    Point point_in_child = point;
    point_in_child.x -= child_position.x + border.left + padding.left;
    point_in_child.y -= child_position.y + border.top + padding.top;

    return point_in_child;
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
        parent->MouseMove(ToParentPoint(position), message);
	}
}


void Control::MouseEnter(const std::shared_ptr<Control>& entered_control) {

    auto parent = GetParent();
    if (parent != nullptr) {
        parent->MouseEnter(entered_control);
    }
}


void Control::MouseLeave(const std::shared_ptr<Control>& leaved_control) {

    auto parent = GetParent();
    if (parent != nullptr) {
        parent->MouseLeave(leaved_control);
    }
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


bool Control::KeyDown(const KeyMessage& message) {

	auto parent = GetParent();
	if (parent != nullptr) {
		return parent->KeyDown(message);
	}
    else {
        return false;
    }
}


bool Control::KeyUp(const KeyMessage& message) {

	auto parent = GetParent();
	if (parent != nullptr) {
		return parent->KeyUp(message);
	}
    else {
        return false;
    }
}


bool Control::CharInput(const CharMessage& message) {

	auto parent = GetParent(); 
	if (parent != nullptr) {
		return parent->CharInput(message);
	}
    else {
        return false;
    }
}


void Control::FocusGain() {

}


void Control::FocusLose() {

}


void Control::SerializeToDataNode(DataNode& data_node) const {

    if (! rect_.IsEmpty()) {
        data_node.AddChild(property::Rect, rect_.Serialize());
    }

    if (! border_.IsEmpty()) {
        data_node.AddChild(property::Border, border_.Serialize());
    }

    if (! padding_.IsEmpty()) {
        data_node.AddChild(property::Padding, padding_.Serialize());
    }

    if (is_visible_ != DefaultIsVisible) {
        data_node.AddChild(property::IsVisible, DataNode::CreateBoolean(is_visible_));
    }

    if (is_enabled_ != DefaultIsEnabled) {
        data_node.AddChild(property::IsEnabled, DataNode::CreateBoolean(is_enabled_));
    }

    if (can_focused_ != DefaultCanFocused) {
        data_node.AddChild(property::CanFocused, DataNode::CreateBoolean(can_focused_));
    }

    SerializeProperties(data_node);
    SerializeChildren(data_node);
}


void Control::SerializeProperties(DataNode& data_node) const {

    GetPropertyMap().EnumerateProperties([&data_node](const std::wstring& name, const PropertyMap::Value& value) {
    
        auto property_data_node = value.Serialize();
        if (property_data_node != nullptr) {
            data_node.AddChild(name, property_data_node);
        }
    });
}


void Control::SerializeChildren(DataNode& data_node) const {

    const auto& children = GetChildren();
    if (children.empty()) {
        return;
    }

    auto children_node = DataNode::CreateArray();
    for (const auto& each_child : children) {
        children_node->AddChild(each_child->Serialize());
    }

    data_node.AddChild(property::Children, children_node);
}


bool Control::DeserializeFromDataNode(const DataNode& data_node) {

    UpdateGuard update_gurad(*this);

    data_node.EnumerateKeyedChildren([this](const std::wstring& key, const DataNode& data_node) {
        DeserializeProperty(key, data_node);
    });

    return true;
}


void Control::DeserializeProperty(const std::wstring& name, const DataNode& data_node) {

    if (name == property::Rect) {
        Rect rect;
        rect.Deserialize(data_node);
        SetRect(rect);
    }
    else if (name == property::BackgroundColor) {
        Color color;
        color.Deserialize(data_node);
        SetBackgroundColor(color);
    }
    else if (name == property::BackgroundColorPicker) {
        ConstantColorPicker color_picker;
        color_picker.Deserialize(data_node);
        SetBackgroundColorPicker(color_picker);
    }
    else if (name == property::Border) {
        Frame border;
        border.Deserialize(data_node);
        SetBorder(border);
    }
    else if (name == property::BorderColor) {
        Color color;
        color.Deserialize(data_node);
        SetBorderColor(color);
    }
    else if (name == property::BorderColorPicker) {
        ConstantColorPicker color_picker;
        color_picker.Deserialize(data_node);
        SetBorderColorPicker(color_picker);
    }
    else if (name == property::Padding) {
        Frame padding;
        padding.Deserialize(data_node);
        SetPadding(padding);
    }
    else if (name == property::IsVisible) {
        SetIsVisible(data_node.GetBoolean());
    }
    else if (name == property::IsEnabled) {
        SetIsEnabled(data_node.GetBoolean());
    }
    else if (name == property::CanFocused) {
        SetCanFocused(data_node.GetBoolean());
    }
    else if (name == property::Name) {
        SetName(data_node.GetString());
    }
    else if (name == property::MinimumWidth) {
        SetMinimumWidth(data_node.GetFloat());
    }
    else if (name == property::MaximumWidth) {
        SetMaximumWidth(data_node.GetFloat());
    }
    else if (name == property::MinimumHeight) {
        SetMinimumHeight(data_node.GetFloat());
    }
    else if (name == property::MaximumHeight) {
        SetMaximumHeight(data_node.GetFloat());
    }
    else if (name == property::Children) {
        DeserializeChildren(data_node);
    }
}


void Control::DeserializeChildren(const DataNode& data_node) {

    RemoveAllChildren();

    data_node.EnumerateChildren([this](const DataNode& data_node) {

        auto type_node = data_node.GetChild(property::Type);
        if (type_node == nullptr) {
            return;
        }

        auto type = GetSerializationManager()->GetType(type_node->GetString());
        if (type == nullptr) {
            return;
        }

        auto instance = type->CreateInstance();
        auto child_control = std::dynamic_pointer_cast<Control>(instance);
        if (child_control == nullptr) {
            return;
        }

        AddChild(child_control);
    });
}


ZAF_DEFINE_TYPE_NAME(Control);

}