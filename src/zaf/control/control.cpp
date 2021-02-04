#include <zaf/control/control.h>
#include <algorithm>
#include <zaf/base/assert.h>
#include <zaf/base/define.h>
#include <zaf/base/event_utility.h>
#include <zaf/control/internal/cached_painting.h>
#include <zaf/control/internal/image_box/image_drawing.h>
#include <zaf/control/layout/anchor_layouter.h>
#include <zaf/graphic/alignment.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/font/font.h>
#include <zaf/graphic/geometry/path_geometry.h>
#include <zaf/graphic/geometry/rectangle_geometry.h>
#include <zaf/graphic/image/image.h>
#include <zaf/internal/theme.h>
#include <zaf/parsing/parsers/control_parser.h>
#include <zaf/reflection/reflection_type_definition.h>
#include <zaf/rx/subject.h>
#include <zaf/serialization/properties.h>
#include <zaf/window/inspector/internal/inspector_port.h>
#include <zaf/window/message/message.h>
#include <zaf/window/message/mouse_message.h>
#include <zaf/window/window.h>

#undef max
#undef min

namespace zaf {
namespace {

const wchar_t* const kAnchorPropertyName = L"Anchor";
const wchar_t* const kBackgroundImageLayoutPropertyName = L"BackgroundImageLayout";
const wchar_t* const kBackgroundImagePickerPropertyName = L"BackgroundImagePicker";
const wchar_t* const kFocusChangeEventPropertyName = L"FocusChangeEvent";
const wchar_t* const kLayouterPropertyName = L"Layouter";
constexpr wchar_t* const kMouseEnterEventPropertyName = L"MouseEnterEvent";
constexpr wchar_t* const kMouseLeaveEventPropertyName = L"MouseLeaveEvent";
const wchar_t* const kRectChangeEventPropertyName = L"RectChangeEvent";

const bool DefaultCanFocused = false;
const bool DefaultIsEnabled = true;
const bool DefaultIsVisible = true;

}


ZAF_DEFINE_REFLECTION_TYPE(Control)
    ZAF_DEFINE_PARSER(ControlParser)
ZAF_DEFINE_END

Control::Control() : 
    update_count_(0),
    need_relayout_after_updating_(false),
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
    need_repaint_rect_after_updating_ = Rect();
    need_relayout_after_updating_ = false;
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

        if (!need_repaint_rect_after_updating_.IsEmpty()) {
            NeedRepaintRect(need_repaint_rect_after_updating_);
        }
    }
}


void Control::Repaint(Canvas& canvas, const Rect& dirty_rect) {

	if (! IsVisible()) {
		return;
	}

    if (IsCachedPaintingEnabled()) {
        RepaintUsingCachedPainting(canvas, dirty_rect);
    }
    else {
        RepaintControl(canvas, dirty_rect, false);
    }
}


void Control::RepaintUsingCachedPainting(Canvas& canvas, const Rect& dirty_rect) {

    auto control_size = GetSize();
    control_size.width = std::ceil(control_size.width);
    control_size.height = std::ceil(control_size.height);

    //Create the cached renderer if it is not created.
    if (cached_renderer_ == nullptr) {
        CreateCompatibleRendererOptions options;
        options.DesiredSize(control_size);
        cached_renderer_ = canvas.GetRenderer().CreateCompatibleRenderer(options);
        valid_cached_renderer_rect_ = Rect{};
    }

    //Calculate the actual dirty rect that needs to repaint, and the new valid rect in cached 
    //renderer.
    auto calculate_result = internal::CalculateDirtyRectOutsideValidCachedRect(
        dirty_rect, 
        valid_cached_renderer_rect_);

    //Repaint the actual dirty rect is it is not empty.
    if (!calculate_result.actual_dirty_rect.IsEmpty()) {

        cached_renderer_.BeginDraw();

        Rect canvas_rect{ Point{}, control_size };
        Canvas cached_painting_canvas(
            cached_renderer_, 
            canvas_rect, 
            calculate_result.actual_dirty_rect);

        RepaintControl(cached_painting_canvas, calculate_result.actual_dirty_rect, true);

        cached_renderer_.EndDraw();

        //Update valid cached rect after repainting.
        valid_cached_renderer_rect_ = calculate_result.new_valid_cached_rect;
    }

    //Paint into canvas from cached renderer.
    canvas.BeginPaint();
    canvas.DrawBitmap(
        cached_renderer_.GetBitmap(),
        dirty_rect, 
        DrawImageOptions().SourceRect(dirty_rect));
    canvas.EndPaint();
}


void Control::RepaintControl(Canvas& canvas, const Rect& dirty_rect, bool need_clear) {

    canvas.BeginPaint();
    if (need_clear) {
        canvas.Clear();
    }
    Paint(canvas, dirty_rect);
    canvas.EndPaint();

    Rect content_rect = GetContentRect();
    if (!content_rect.HasIntersection(dirty_rect)) {
        return;
    }

    const auto& border = GetBorder();
    const auto& padding = GetPadding();

    for (const auto& child : children_) {

        Rect child_rect = child->GetRect();
        child_rect.position.x += border.left + padding.left;
        child_rect.position.y += border.top + padding.top;

        Rect child_dirty_rect = Rect::Intersect(child_rect, dirty_rect);
        if (child_dirty_rect.IsEmpty()) {
            continue;
        }

        canvas.PushTransformLayer(child_rect, content_rect);
        child_dirty_rect.position.x -= child_rect.position.x;
        child_dirty_rect.position.y -= child_rect.position.y;
        child->Repaint(canvas, child_dirty_rect);
        canvas.PopTransformLayer();
    }
}


void Control::Paint(Canvas& canvas, const Rect& dirty_rect) {

	Canvas::StateGuard state_guard(canvas);

    Rect control_rect{ Point{}, GetSize() };

    //Draw background color.
    canvas.SetBrushWithColor(GetBackgroundColor());
    canvas.DrawRectangle(control_rect);

    //Draw background image.
    DrawBackgroundImage(canvas, control_rect);

    //Calculate border geometry and draw border.
    Rect inner_rect = control_rect;
    inner_rect.Deflate(GetBorder());

    //The with and height must be greater than 0.
    if (inner_rect.size.width < 0) {
        inner_rect.size.width = 0;
    }
    if (inner_rect.size.height < 0) {
        inner_rect.size.height = 0;
    }

    auto control_geometry = canvas.CreateRectangleGeometry(control_rect);
    auto inner_geometry = canvas.CreateRectangleGeometry(inner_rect);
    
    auto border_geometry = canvas.CreatePathGeometry();
    auto sink = border_geometry.Open();
    Geometry::Combine(control_geometry, inner_geometry, Geometry::CombineMode::Exclude, sink);
    sink.Close();

	canvas.SetBrushWithColor(GetBorderColor());
    canvas.DrawGeometry(border_geometry);
}


void Control::DrawBackgroundImage(Canvas& canvas, const Rect& background_rect) {

    auto background_image = GetBackgroundImage();
    if (!background_image) {
        return;
    }
    
    auto render_bitmap = background_image->CreateRenderBitmap(canvas.GetRenderer());
    if (render_bitmap == nullptr) {
        return;
    }

    internal::DrawImage(
        canvas,
        background_rect,
        GetBackgroundImageLayout(),
        render_bitmap,
        InterpolationMode::Linear);
}


void Control::NeedRepaint() {
	NeedRepaintRect(Rect(Point(), rect_.size));
}


void Control::NeedRepaintRect(const Rect& rect) {

    if ((rect.size.width == 0) || (rect.size.height == 0)) {
        return;
    }

    if (IsUpdating()) {
        need_repaint_rect_after_updating_.Union(rect);
        return;
    }

	Rect bound_rect(Point(), rect_.size);
	Rect repaint_rect = Rect::Intersect(bound_rect, rect);
	if (repaint_rect.IsEmpty()) {
		return;
	}

    RecalculateCachedPaintingRect(repaint_rect);

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


void Control::RecalculateCachedPaintingRect(const Rect& repaint_rect) {

    if (cached_renderer_ == nullptr) {
        return;
    }

    Rect invalid_rect = internal::CalculateInvalidRectInCachedRect(
        valid_cached_renderer_rect_, 
        repaint_rect);

    if (invalid_rect.IsEmpty()) {
        return;
    }

    valid_cached_renderer_rect_.Subtract(invalid_rect);
}


void Control::ReleaseRendererResources() {

    ReleaseCachedPaintingRenderer();

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
	if (layouter) {
		layouter->Layout(*this, previous_rect, GetChildren());
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


Rect Control::GetAbsoluteRect() const {

	auto window = GetWindow();
	if (window == nullptr) {
		return Rect();
	}

    //No parent, must be the root control, return its rect as the absolute rect.
	auto parent = GetParent();
	if (parent == nullptr) {
		return GetRect();
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

    //Notify rect change.
    OnRectChanged(previous_rect);

    if (rect_.size != previous_rect.size) {

        ReleaseCachedPaintingRenderer();

        //Layout children if size is changed.
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
		parent->ChildRectChanged(shared_from_this(), previous_rect);
	}

    //Trigger the rect change event.
    auto event_observer = GetEventObserver<ControlRectChangeInfo>(
        GetPropertyMap(),
        kRectChangeEventPropertyName);

    if (event_observer) {
        ControlRectChangeInfo event_info;
        event_info.control = shared_from_this();
        event_info.previous_rect = previous_rect;
        event_observer->OnNext(event_info);
    }
}


void Control::OnRectChanged(const Rect& previous_rect) {

}


void Control::SetMargin(const Frame& margin) {

    margin_ = margin;

    //Notify parent to re-layout all children.
    auto parent = GetParent();
    if (parent) {
        parent->NeedRelayout();
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


Size Control::GetPreferredSize() const {

    auto result = GetPreferredContentSize();

    const auto& padding = GetPadding();
    result.width += padding.left + padding.right;
    result.height += padding.top + padding.bottom;

    const auto& border = GetBorder();
    result.width += border.left + border.right;
    result.height += border.top + border.bottom;

    return EnforceSizeLimit(result);
}


Size Control::GetPreferredContentSize() const {

    Rect union_rect;

    for (const auto& each_child : GetChildren()) {

        auto child_rect = each_child->GetRect();
        child_rect.Inflate(each_child->GetMargin());

        Rect needed_rect;
        needed_rect.size.width = std::max(child_rect.position.x + child_rect.size.width, 0.f);
        needed_rect.size.height = std::max(child_rect.position.y + child_rect.size.height, 0.f);

        union_rect.Union(needed_rect);
    }

    return union_rect.size;
}


Size Control::EnforceSizeLimit(const Size& size) const {

    Size result = size;

    result.width = std::min(result.width, GetMaximumWidth());
    result.width = std::max(result.width, GetMinimumWidth());

    result.height = std::min(result.height, GetMaximumHeight());
    result.height = std::max(result.height, GetMinimumHeight());

    return result;
}


void Control::ResizeToPreferredSize() {

    auto preferred_size = GetPreferredSize();
    SetSize(preferred_size);
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


ImagePicker Control::GetBackgroundImagePicker() const {

    auto image_picker = GetPropertyMap().TryGetProperty<ImagePicker>(
        kBackgroundImagePickerPropertyName);

    if (image_picker && *image_picker) {
        return *image_picker;
    }
    return CreateImagePicker(nullptr);
}


void Control::SetBackgroundImagePicker(const ImagePicker& image_picker) {

    GetPropertyMap().SetProperty(kBackgroundImagePickerPropertyName, image_picker);
    NeedRepaint();
}


ImageLayout Control::GetBackgroundImageLayout() const {

    auto layout = GetPropertyMap().TryGetProperty<ImageLayout>(kBackgroundImageLayoutPropertyName);
    if (layout) {
        return *layout;
    }
    return ImageLayout::None;
}

void Control::SetBackgroundImageLayout(ImageLayout image_layout) {

    GetPropertyMap().SetProperty(kBackgroundImageLayoutPropertyName, image_layout);
    NeedRepaint();
}


ColorPicker Control::GetBackgroundColorPicker() const {

	auto color_picker = GetPropertyMap().TryGetProperty<ColorPicker>(property::BackgroundColorPicker);
	if ((color_picker != nullptr) && (*color_picker != nullptr)) {
		return *color_picker;
	}
    else {
        return [](const Control&) {
            return Color::Transparent();
        };
    }
}


void Control::SetBackgroundColorPicker(const ColorPicker& color_picker) {

	GetPropertyMap().SetProperty(property::BackgroundColorPicker, color_picker);
	NeedRepaint();
}


ColorPicker Control::GetBorderColorPicker() const {

	auto color_picker = GetPropertyMap().TryGetProperty<ColorPicker>(property::BorderColorPicker);
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

	GetPropertyMap().SetProperty(property::BorderColorPicker, color_picker);
	NeedRepaint();
}


std::shared_ptr<Layouter> Control::GetLayouter() const {

	auto layouter = GetPropertyMap().TryGetProperty<std::shared_ptr<Layouter>>(
        kLayouterPropertyName
    );
	if (layouter && *layouter) {
		return *layouter;
	}
	else {
		return GetAnchorLayouter();
	}
}

void Control::SetLayouter(const std::shared_ptr<Layouter>& layouter) {
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

    auto inspector_port = GetInspectorPort();
    if (inspector_port) {
        inspector_port->ControlAddChild(shared_from_this());
    }
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

    auto removed_iterator = std::find(children_.begin(), children_.end(), child);
    if (removed_iterator == children_.end()) {
        return;
    }

    auto removed_index = std::distance(children_.begin(), removed_iterator);
	children_.erase(removed_iterator);

    //The child's rect may be changed while calling NeedRelayout(), leading to a wrong repaint rect
    //while calling NeedRepaintRect(), so we preserve the original rect before calling 
    //NeedRelayout().
    auto child_rect = child->GetRect();

    NeedRelayout();
	NeedRepaintRect(child_rect);

    auto inspector_port = GetInspectorPort();
    if (inspector_port) {
        inspector_port->ControlRemoveChild(shared_from_this(), removed_index);
    }
}


void Control::RemoveAllChildren() {

    for (const auto& each_child : children_) {
        each_child->SetParent(nullptr);
    }

    children_.clear();
    NeedRepaint();
}


std::shared_ptr<Control> Control::FindChild(const std::wstring& name) const {

    for (const auto& each_child : children_) {
        if (each_child->GetName() == name) {
            return each_child;
        }
    }

    for (const auto& each_child : children_) {
        auto found_child = each_child->FindChild(name);
        if (found_child) {
            return found_child;
        }
    }

    return {};
}


std::shared_ptr<Control> Control::InnerFindChildAtPosition(
    const Point& position, 
    bool recursively) const {

    auto content_rect = GetContentRect();

    auto position_in_content = position;
    position_in_content.x -= content_rect.position.x;
    position_in_content.y -= content_rect.position.y;

    content_rect.position.x = 0;
    content_rect.position.y = 0;

    for (auto iterator = children_.rbegin(); iterator != children_.rend(); ++iterator) {

        const auto& child = *iterator;

        if (!child->IsVisible()) {
            continue;
        }

        Rect child_rect = child->GetRect();
        child_rect.Intersect(content_rect);

        if (!child_rect.Contain(position_in_content)) {
            continue;
        }

        if (!recursively) {
            return child;
        }

        auto recursive_child = child->InnerFindChildAtPosition(ToChildPoint(position, child), true);
        if (recursive_child) {
            return recursive_child;
        }

        return child;
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


std::wstring Control::GetName() const {

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


std::shared_ptr<Window> Control::GetWindow() const {

	auto parent = GetParent();
	if (parent == nullptr) {
		return window_.lock();
	}

	return parent->GetWindow();
}


bool Control::IsVisible() const {

    if (! is_visible_) {
        return false;
    }

    auto parent = GetParent(); 
    if (parent == nullptr) {
        return true;
    }

    return parent->IsVisible();
}


void Control::SetIsVisible(bool is_visible) {
    SetInteractiveProperty(is_visible, is_visible_, &Control::OnIsVisibleChanged);
}


void Control::OnIsVisibleChanged() {

    for (const auto& each_child : children_) {
        each_child->OnIsVisibleChanged();
    }
}


bool Control::IsEnabled() const {

    if (! is_enabled_) {
        return false;
    }

    auto parent = GetParent();
    if (parent == nullptr) {
        return true;    
    }

    return parent->IsEnabled();   
}


void Control::SetIsEnabled(bool is_enabled) {
    SetInteractiveProperty(is_enabled, is_enabled_, &Control::OnIsEnabledChanged);
}


void Control::OnIsEnabledChanged() {

    for (const auto& each_child : children_) {
        each_child->OnIsEnabledChanged();
    }
}


void Control::SetInteractiveProperty(bool new_value, bool& property_value, void(Control::*notification)()) {

    if (property_value == new_value) {
        return;
    }

    if (! new_value) {

        if (IsHovered()) {
            auto window = GetWindow();
            if (window != nullptr) {
                window->SetHoveredControl(nullptr, {});
            }
        }

        SetIsFocused(false);
    }

    property_value = new_value;
    NeedRepaint();

    if (notification != nullptr) {
        (this->*notification)();
    }
}


bool Control::IsSelected() const {
    
    if (is_selected_) {
        return true;
    }

    auto parent = GetParent();
    if (!parent) {
        return false;
    }

    return parent->IsSelected();
}


void Control::SetIsSelected(bool is_selected) {

    if (is_selected_ == is_selected) {
        return;
    }

    is_selected_ = is_selected;
    NeedRepaint();
}


void Control::OnIsSelectedChanged() {

    for (const auto& each_child : children_) {
        each_child->OnIsSelectedChanged();
    }
}


void Control::IsHoveredChanged(bool is_hovered) {

	if (is_hovered_ == is_hovered) {
		return;
	}

    auto point = GetMousePosition();
    
	is_hovered_ = is_hovered;

	if (is_hovered_) {
		OnMouseEnter(shared_from_this());
	}
	else {
        OnMouseLeave(shared_from_this());
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


std::optional<std::size_t> Control::GetTabIndex() const {

    auto tab_index = GetPropertyMap().TryGetProperty<std::size_t>(property::TabIndex);
    if (tab_index != nullptr) {
        return *tab_index;
    }
    else {
        return std::nullopt;
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
		OnFocusGain();
	}
	else {
		OnFocusLose();
	}

    auto event_observer = GetEventObserver<ControlFocusChangeInfo>(
        GetPropertyMap(),
        kFocusChangeEventPropertyName);

    if (event_observer) {
        ControlFocusChangeInfo event_info;
        event_info.control = shared_from_this();
        event_observer->OnNext(event_info);
    }
}


void Control::SetIsCachedPaintingEnabled(bool value) {

    if (is_cached_painting_enabled_ == value) {
        return;
    }

    is_cached_painting_enabled_ = value;
    ReleaseCachedPaintingRenderer();
}


void Control::ReleaseCachedPaintingRenderer() {
    cached_renderer_ = nullptr;
    valid_cached_renderer_rect_ = {};
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
		OnMouseCapture();
	}
	else {
		OnMouseRelease();
	}
}


Observable<ControlRectChangeInfo> Control::RectChangeEvent() {
    return GetEventObservable<ControlRectChangeInfo>(
        GetPropertyMap(), 
        kRectChangeEventPropertyName);
}


Observable<ControlFocusChangeInfo> Control::FocusChangeEvent() {
    return GetEventObservable<ControlFocusChangeInfo>(
        GetPropertyMap(),
        kFocusChangeEventPropertyName);
}


Observable<ControlMouseEnterInfo> Control::MouseEnterEvent() {
    return GetEventObservable<ControlMouseEnterInfo>(
        GetPropertyMap(), 
        kMouseEnterEventPropertyName);
}


Observable<ControlMouseLeaveInfo> Control::MouseLeaveEvent() {
    return GetEventObservable<ControlMouseLeaveInfo>(
        GetPropertyMap(), 
        kMouseLeaveEventPropertyName);
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


void Control::RouteHoverMessage(const Point& position, const MouseMessage& message) {

	auto child = FindChildAtPosition(position);
	if (child != nullptr) {
		child->RouteHoverMessage(ToChildPoint(position, child), message);
	}
	else {

		auto window = GetWindow();
		if (window != nullptr) {

			if (IsEnabled()) {
				window->SetHoveredControl(shared_from_this(), message);
			}
			else {
                window->SetHoveredControl(nullptr, {});
			}
		}
	}
}


bool Control::RouteMessage(const Point& position, const MouseMessage& message) {

	auto child = FindChildAtPosition(position);
	if (child != nullptr) {
        return child->RouteMessage(ToChildPoint(position, child), message);
	}
	
	if (IsEnabled()) {
        return InterpretMessage(position, message);
	}
    return false;
}


Point Control::ToChildPoint(const Point& point, const std::shared_ptr<Control>& child) const {

    const auto& border = GetBorder();
    const auto& padding = GetPadding();
    const auto& child_position = child->GetPosition();

    Point point_in_child = point;
    point_in_child.x -= child_position.x + border.left + padding.left;
    point_in_child.y -= child_position.y + border.top + padding.top;

    return point_in_child;
}


bool Control::InterpretMessage(const Point& position, const MouseMessage& message) {

	switch (message.id) {
	case WM_MOUSEMOVE:
		return OnMouseMove(position, message);

	case WM_LBUTTONDOWN:
    case WM_NCLBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
        return OnMouseDown(position, message);

	case WM_LBUTTONUP:
    case WM_NCLBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
        return OnMouseUp(position, message);

	case WM_MOUSEWHEEL:
	case WM_MOUSEHWHEEL:
        return OnMouseWheel(position, dynamic_cast<const MouseWheelMessage&>(message));

	default:
        return false;
	}
}


std::optional<HitTestResult> Control::HitTest(const HitTestMessage& message) {

    auto parent = GetParent();
    if (parent != nullptr) {
        return parent->HitTest(message);
    }
    else {
        return {};
    }
}


void Control::ChangeMouseCursor(const Message& message, bool& is_changed) {

	auto parent = GetParent();
	if (parent != nullptr) {
		parent->ChangeMouseCursor(message, is_changed);
	}
}


bool Control::OnMouseMove(const Point& position, const MouseMessage& message) {

	auto parent = GetParent();
	if (parent != nullptr) {
        return parent->OnMouseMove(ToParentPoint(position), message);
	}
    else {
        return false;
    }
}


void Control::OnMouseEnter(const std::shared_ptr<Control>& entered_control) {

    auto event_observer = GetEventObserver<ControlMouseEnterInfo>(
        GetPropertyMap(), 
        kMouseEnterEventPropertyName);

    if (event_observer) {
        ControlMouseEnterInfo event_info;
        event_info.control = shared_from_this();
        event_info.entered_control = entered_control;
        event_observer->OnNext(event_info);
    }

    auto parent = GetParent();
    if (parent != nullptr) {
        parent->OnMouseEnter(entered_control);
    }
}


void Control::OnMouseLeave(const std::shared_ptr<Control>& leaved_control) {

    auto event_observer = GetEventObserver<ControlMouseLeaveInfo>(
        GetPropertyMap(),
        kMouseLeaveEventPropertyName);

    if (event_observer) {
        ControlMouseLeaveInfo event_info;
        event_info.control = shared_from_this();
        event_info.leaved_control = leaved_control;
        event_observer->OnNext(event_info);
    }

    auto parent = GetParent();
    if (parent != nullptr) {
        parent->OnMouseLeave(leaved_control);
    }
}


bool Control::OnMouseDown(const Point& position, const MouseMessage& message) {

	auto parent = GetParent();
	if (parent != nullptr) {
        return parent->OnMouseDown(ToParentPoint(position), message);
	}
    else {
        return false;
    }
}


bool Control::OnMouseUp(const Point& position, const MouseMessage& message) {

	auto parent = GetParent();
	if (parent != nullptr) {
        return parent->OnMouseUp(ToParentPoint(position), message);
	}
    else {
        return false;
    }
}


bool Control::OnMouseWheel(const Point& position, const MouseWheelMessage& message) {

	auto parent = GetParent();
	if (parent != nullptr) {
		return parent->OnMouseWheel(ToParentPoint(position), message);
	}
    else {
        return false;
    }
}


void Control::OnMouseCapture() {

}


void Control::OnMouseRelease() {

}


bool Control::OnKeyDown(const KeyMessage& message) {

	auto parent = GetParent();
	if (parent != nullptr) {
		return parent->OnKeyDown(message);
	}
    else {
        return false;
    }
}


bool Control::OnKeyUp(const KeyMessage& message) {

	auto parent = GetParent();
	if (parent != nullptr) {
		return parent->OnKeyUp(message);
	}
    else {
        return false;
    }
}


bool Control::OnCharInput(const CharMessage& message) {

	auto parent = GetParent(); 
	if (parent != nullptr) {
		return parent->OnCharInput(message);
	}
    else {
        return false;
    }
}


void Control::OnFocusGain() {

}


void Control::OnFocusLose() {

}


std::shared_ptr<internal::InspectorPort> Control::GetInspectorPort() const {

    auto window = GetWindow();
    if (!window) {
        return nullptr;
    }

    return window->GetInspectorPort();
}

}