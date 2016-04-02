#include <zaf/control/control.h>
#include <algorithm>
#include <zaf/application.h>
#include <zaf/base/assert.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/font/font.h>
#include <zaf/graphic/text/text_format.h>
#include <zaf/graphic/text/text_format_properties.h>
#include <zaf/graphic/text/text_layout.h>
#include <zaf/graphic/text/text_layout_properties.h>
#include <zaf/window/window.h>

namespace zaf {

static Point ToChildPoint(const Point& point, const std::shared_ptr<Control>& child);
static std::wstring CreateColorPropertyName(int paint_component, int paint_state);

static const wchar_t* const kAnchorPropertyName = L"Anchor";
static const wchar_t* const kFontPropertyName = L"Font";
static const wchar_t* const kNamePropertyName = L"Name";
static const wchar_t* const kParagraphAlignmentPropertyName = L"ParagraphAlignment";
static const wchar_t* const kTextAlignmentPropertyName = L"TextAlignment";
static const wchar_t* const kTextPropertyName = L"Text";
static const wchar_t* const kWordWrappingPropertyName = L"WordWrapping";

Control::Control() : 
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

	int paint_state = GetPaintState();

	Rect paint_rect(Point(), GetSize());
	canvas.SetBrushWithColor(GetColor(PaintComponent::Border, paint_state));
	canvas.DrawRectangle(paint_rect);

	paint_rect.Inflate(-GetBorderWidth());
	canvas.SetBrushWithColor(GetColor(PaintComponent::Background, paint_state));
	canvas.DrawRectangle(paint_rect);
}


int Control::GetPaintState() const {

	if (!IsEnabled()) {
		return PaintState::Disabled;
	}

	if (IsHovered()) {
		return PaintState::Hovered;
	}

	if (IsFocused()) {
		return PaintState::Focused;
	}

	return PaintState::Normal;
}


std::shared_ptr<TextFormat> Control::CreateTextFormat() const {

	auto font = GetFont();
	TextFormatProperties text_format_properties;
	text_format_properties.font_family_name = font.family_name;
	text_format_properties.font_size = font.size;
	text_format_properties.font_weight = font.weight;

	auto text_format = Application::GetInstance().GetRendererFactory()->CreateTextFormat(text_format_properties);
	if (text_format == nullptr) {
		return nullptr;
	}

	text_format->SetTextAlignment(GetTextAlignment());
	text_format->SetParagraphAlignment(GetParagraphAlignment());
	text_format->SetWordWrapping(GetWordWrapping());

	return text_format;
}


std::shared_ptr<TextLayout> Control::CreateTextLayout(const Size& layout_size) const {

	auto text_format = CreateTextFormat();
	if (text_format == nullptr) {
		return nullptr;
	}

	TextLayoutProperties text_layout_properties;
	text_layout_properties.text = GetText();
	text_layout_properties.text_format = text_format;
	text_layout_properties.width = layout_size.width;
	text_layout_properties.height = layout_size.height;
	return Application::GetInstance().GetRendererFactory()->CreateTextLayout(text_layout_properties);
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

	for (auto& child : children_) {
		LayoutChild(child, previous_rect);
	}
}


void Control::LayoutChild(const std::shared_ptr<Control>& child, const Rect& previous_rect) {

	auto change_single_dimension_with_anchor = [](
		bool has_front_anchor,
		bool has_back_anchor,
		float parent_old_size,
		float parent_new_size,
		float old_position,
		float old_size,
		float& new_position,
		float& new_size
	) {

		if (has_front_anchor && ! has_back_anchor) {
			new_position = old_position;
			new_size = old_size;
		}
		else if (! has_front_anchor && has_back_anchor) {
			float old_back = parent_old_size - old_position - old_size;
			new_position = parent_new_size - old_back - old_size;
			new_size = old_size;
		}
		else if (has_front_anchor && has_back_anchor) {
			new_position = old_position;
			float old_back = parent_old_size - old_position - old_size;
			new_size = parent_new_size - old_back - new_position;
		}
		else {
			new_position = old_position;
			new_size = old_size;
		}
	};

	const Rect& current_rect = GetRect();

	Anchor anchor = child->GetAnchor();
	bool has_left_anchor = (anchor & Anchor::Left) == Anchor::Left;
	bool has_right_anchor = (anchor & Anchor::Right) == Anchor::Right;
	bool has_top_anchor = (anchor & Anchor::Top) == Anchor::Top;
	bool has_bottom_anchor = (anchor & Anchor::Bottom) == Anchor::Bottom;
	
	const Rect& child_old_rect = child->GetRect();
	Rect child_new_rect;

	change_single_dimension_with_anchor(
		has_left_anchor,
		has_right_anchor,
		previous_rect.size.width,
		current_rect.size.width,
		child_old_rect.position.x,
		child_old_rect.size.width,
		child_new_rect.position.x,
		child_new_rect.size.width
	);

	change_single_dimension_with_anchor(
		has_top_anchor,
		has_bottom_anchor,
		previous_rect.size.height,
		current_rect.size.height,
		child_old_rect.position.y,
		child_old_rect.size.height,
		child_new_rect.position.y,
		child_new_rect.size.height
	);

	child->SetRect(child_new_rect);
}


void Control::NeedRelayout() {
	Layout(GetRect());
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


const Color Control::GetColor(int paint_component, int paint_state) const {

	auto color_property_name = CreateColorPropertyName(paint_component, paint_state);
	auto color = GetPropertyMap().TryGetProperty<Color>(color_property_name);
	if (color != nullptr) {
		return *color;
	}

	return GetDefaultColor(paint_component, paint_state);
}


const Color Control::GetDefaultColor(int paint_component, int paint_state) const {

	switch (paint_component) {

		case PaintComponent::Background:
			return GetDefaultBackgroundColor(paint_state);

		case PaintComponent::Foreground:
			return GetDefaultForegroundColor(paint_state);

		case PaintComponent::Border:
			return GetDefaultBorderColor(paint_state);

		default:
			ZAF_FAIL();
			return GetDefaultBackgroundColor(paint_state);
	}
}


const Color Control::GetDefaultBackgroundColor(int paint_state) const {

	switch (paint_state) {

		case PaintState::Normal: {

			auto parent = GetParent();
			if (parent != nullptr) {
				return parent->GetColor(PaintComponent::Background, PaintState::Normal);
			}

			return Color::White;
		}

		case PaintState::Hovered: 
		case PaintState::Focused:
		case PaintState::Disabled:
			return GetColor(PaintComponent::Background, PaintState::Normal);

		default:
			ZAF_FAIL();
			return GetColor(PaintComponent::Background, PaintState::Normal);
	}
}


const Color Control::GetDefaultForegroundColor(int paint_state) const {

	switch (paint_state) {

		case PaintState::Normal: 
			return Color::Black;

		case PaintState::Hovered:
		case PaintState::Focused:
		case PaintState::Disabled:
			return GetColor(PaintComponent::Foreground, PaintState::Normal);
			
		default:
			ZAF_FAIL();
			return GetColor(PaintComponent::Foreground, PaintState::Normal);
	}
}


const Color Control::GetDefaultBorderColor(int paint_state) const {

	switch (paint_state) {

		case PaintState::Normal:
			return GetColor(PaintComponent::Background, PaintState::Normal);

		case PaintState::Hovered:
		case PaintState::Focused:
		case PaintState::Disabled:
			return GetColor(PaintComponent::Border, PaintState::Normal);

		default:
			ZAF_FAIL();
			return GetColor(PaintComponent::Border, PaintState::Normal);
	}
}


void Control::SetColor(int paint_component, int paint_state, const Color& color) {

	auto color_property_name = CreateColorPropertyName(paint_component, paint_state);
	GetPropertyMap().SetProperty(color_property_name, color);
	NeedRepaint();
}


const std::wstring Control::GetText() const {

	auto text = GetPropertyMap().TryGetProperty<std::wstring>(kTextPropertyName);
	if (text != nullptr) {
		return *text;
	}
	else {
		return std::wstring();
	}
}


void Control::SetText(const std::wstring& text) {
	GetPropertyMap().SetProperty(kTextPropertyName, text);
	NeedRepaint();
}


const Font Control::GetFont() const {

	auto font = GetPropertyMap().TryGetProperty<Font>(kFontPropertyName);
	if (font != nullptr) {
		return *font;
	}
	else {

		auto parent = GetParent();
		if (parent != nullptr) {
			return parent->GetFont();
		}

		return Font();
	};
}


void Control::SetFont(const Font& font) {
	GetPropertyMap().SetProperty(kFontPropertyName, font);
	NeedRepaint();
}


TextAlignment Control::GetTextAlignment() const {

	auto text_alignment = GetPropertyMap().TryGetProperty<TextAlignment>(kTextAlignmentPropertyName);
	if (text_alignment != nullptr) {
		return *text_alignment;
	}
	else {
		return TextAlignment::Leading;
	}
}


void Control::SetTextAlignment(TextAlignment alignment) {
	GetPropertyMap().SetProperty(kTextAlignmentPropertyName, alignment);
	NeedRepaint();
}


ParagraphAlignment Control::GetParagraphAlignment() const {

	auto paragraph_alignment = GetPropertyMap().TryGetProperty<ParagraphAlignment>(kParagraphAlignmentPropertyName);
	if (paragraph_alignment != nullptr) {
		return *paragraph_alignment;
	}
	else {
		return ParagraphAlignment::Near;
	}
}


void Control::SetParagraphAlignment(ParagraphAlignment alignment) {
	GetPropertyMap().SetProperty(kParagraphAlignmentPropertyName, alignment);
	NeedRepaint();
}


WordWrapping Control::GetWordWrapping() const {

	auto word_wrapping = GetPropertyMap().TryGetProperty<WordWrapping>(kWordWrappingPropertyName);
	if (word_wrapping != nullptr) {
		return *word_wrapping;
	}
	else {
		return WordWrapping::NoWrap;
	}
}


void Control::SetWordWrapping(WordWrapping word_wrapping) {
	GetPropertyMap().SetProperty(kWordWrappingPropertyName, word_wrapping);
	NeedRepaint();
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

		Rect child_rect = child->GetRect();
		child_rect.Intersect(GetContentRect());

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


static std::wstring CreateColorPropertyName(int paint_component, int paint_state) {

	std::wstring color_property_name(L"Color_");
	color_property_name.append(std::to_wstring(paint_component));
	color_property_name.append(1, L'_');
	color_property_name.append(std::to_wstring(paint_state));
	return color_property_name;
}

}