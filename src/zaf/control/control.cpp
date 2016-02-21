#include <zaf/control/control.h>
#include <algorithm>
#include <zaf/application.h>
#include <zaf/base/assert.h>
#include <zaf/base/log.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/text/font.h>
#include <zaf/window/window.h>

namespace zaf {

static Point ToChildPoint(const Point& point, const std::shared_ptr<Control>& child);

static const wchar_t* const kAnchorPropertyName = L"Anchor";
static const wchar_t* const kBackgroundColorPropertyName = L"BackgroundColor";
static const wchar_t* const kBorderColorPropertyName = L"BorderColor";
static const wchar_t* const kDisabledBackgroundColorPropertyName = L"DisabledBackgroundColor";
static const wchar_t* const kDisabledBorderColorPropertyName = L"DisabledBorderColor";
static const wchar_t* const kDisabledForegroundColorPropertyName = L"DisabledForegroundColor";
static const wchar_t* const kForegroundColorPropertyName = L"ForegroundColor";
static const wchar_t* const kFocusedBackgroundColorPropertyName = L"FocusedBackgroundColor";
static const wchar_t* const kFocusedBorderColorPropertyName = L"FocusedBorderColor";
static const wchar_t* const kFocusedForegroundColorPropertyName = L"FocusedForegroundColor";
static const wchar_t* const kHoveredBackgroundColorPropertyName = L"HoveredBackgroundColor";
static const wchar_t* const kHoveredBorderColorPropertyName = L"HoveredBorderColor";
static const wchar_t* const kHoveredForegroundColorPropertyName = L"HoveredForegroundColor";
static const wchar_t* const kNamePropertyName = L"Name";
static const wchar_t* const kParagraphAlignmentPropertyName = L"ParagraphAlignment";
static const wchar_t* const kTextAlignmentPropertyName = L"TextAlignment";
static const wchar_t* const kTextPropertyName = L"Text";
static const wchar_t* const kWordWrappingPropertyName = L"WordWrapping";

Control::Control() : 
	is_initialized_(false),
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


void Control::PaintText(Canvas& canvas, const Rect& dirty_rect, const Rect& text_rect) {

	std::wstring text = GetText();
	if (text.empty()) {
		return;
	}

	Font font;
	font.family_name = L"Î¢ÈíÑÅºÚ";
	font.size = 13;

	auto text_format = canvas.CreateTextFormat(font);
	if (text_format == nullptr) {
		return;
	}

	text_format->SetTextAlignment(GetTextAlignment());
	text_format->SetParagraphAlignment(GetParagraphAlignment());
	text_format->SetWordWrapping(GetWordWrapping());

	canvas.SetBrushWithColor(GetColor(PaintComponent::Foreground, GetPaintState()));
	canvas.DrawText(GetText(), text_format, text_rect);
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

	auto anchor = property_map_.TryGetProperty<Anchor>(kAnchorPropertyName);
	if (anchor != nullptr) {
		return *anchor;
	}
	return Anchor::None;
}


void Control::SetAnchor(Anchor anchor) {
	property_map_.SetProperty(kAnchorPropertyName, anchor);
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

	switch (paint_component) {

		case PaintComponent::Background:
			return GetBackgroundColor(paint_state);

		case PaintComponent::Foreground:
			return GetForegroundColor(paint_state);

		case PaintComponent::Border:
			return GetBorderColor(paint_state);

		default:
			ZAFFAIL();
			return GetBackgroundColor(PaintState::Normal);
	}
}


const Color Control::GetBackgroundColor(int paint_state) const {

	auto get_normal_background_color = [this]() {
		return GetBackgroundColor(PaintState::Normal);
	};

	switch (paint_state) {

		case PaintState::Normal: 
			return GetPropertyColor(kBackgroundColorPropertyName, [this]() {
			
				auto parent = GetParent();
				if (parent != nullptr) {
					return parent->GetColor(PaintComponent::Background, PaintState::Normal);
				}

				return Color::White;
			});

		case PaintState::Hovered: 
			return GetPropertyColor(kHoveredBackgroundColorPropertyName, get_normal_background_color);

		case PaintState::Focused:
			return GetPropertyColor(kFocusedBackgroundColorPropertyName, get_normal_background_color);

		case PaintState::Disabled:
			return GetPropertyColor(kDisabledBackgroundColorPropertyName, get_normal_background_color);

		default:
			ZAFFAIL();
			return GetBackgroundColor(PaintState::Normal);
	}
}


const Color Control::GetForegroundColor(int paint_state) const {

	auto get_normal_foreground_color = [this]() {
		return GetForegroundColor(PaintState::Normal);
	};

	switch (paint_state) {

		case PaintState::Normal: 
			return GetPropertyColor(kForegroundColorPropertyName, []() {
				return Color::Black;
			});

		case PaintState::Hovered:
			return GetPropertyColor(kHoveredForegroundColorPropertyName, get_normal_foreground_color);

		case PaintState::Focused:
			return GetPropertyColor(kFocusedForegroundColorPropertyName, get_normal_foreground_color);

		case PaintState::Disabled:
			return GetPropertyColor(kDisabledForegroundColorPropertyName, get_normal_foreground_color);

		default:
			ZAFFAIL();
			return GetForegroundColor(PaintState::Normal);
	}
}


const Color Control::GetBorderColor(int paint_state) const {

	auto get_normal_border_color = [this]() {
		return GetBorderColor(PaintState::Normal);
	};

	switch (paint_state) {

		case PaintState::Normal:
			return GetPropertyColor(kBorderColorPropertyName, [this]() {
				return GetBackgroundColor(PaintState::Normal);
			});

		case PaintState::Hovered:
			return GetPropertyColor(kHoveredBorderColorPropertyName, get_normal_border_color);
			
		case PaintState::Focused:
			return GetPropertyColor(kFocusedBorderColorPropertyName, get_normal_border_color);

		case PaintState::Disabled:
			return GetPropertyColor(kDisabledBorderColorPropertyName, get_normal_border_color);

		default:
			ZAFFAIL();
			return GetBorderColor(PaintState::Normal);
	}
}


const Color Control::GetPropertyColor(
	const std::wstring& color_property_name,
	const std::function<const Color()>& get_default_color
) const {

	auto color = property_map_.TryGetProperty<Color>(color_property_name);
	if (color != nullptr) {
		return *color;
	}

	return get_default_color();
}


void Control::SetColor(int paint_component, int paint_state, const Color& color) {

	std::wstring property_name;

	switch (paint_component) {

		case PaintComponent::Background:
			switch (paint_state) {
				case PaintState::Normal:
					property_name = kBackgroundColorPropertyName;
					break;
				case PaintState::Hovered:
					property_name = kHoveredBackgroundColorPropertyName;
					break;
				case PaintState::Focused:
					property_name = kFocusedBackgroundColorPropertyName;
					break;
				case PaintState::Disabled:
					property_name = kDisabledBackgroundColorPropertyName;
					break;
			}
			break;

		case PaintComponent::Foreground:
			switch (paint_state) {
				case PaintState::Normal:
					property_name = kForegroundColorPropertyName;
					break;
				case PaintState::Hovered:
					property_name = kHoveredForegroundColorPropertyName;
					break;
				case PaintState::Focused:
					property_name = kFocusedForegroundColorPropertyName;
					break;
				case PaintState::Disabled:
					property_name = kDisabledForegroundColorPropertyName;
					break;
			}
			break;

		case PaintComponent::Border:
			switch (paint_state) {
				case PaintState::Normal:
					property_name = kBorderColorPropertyName;
					break;
				case PaintState::Hovered:
					property_name = kHoveredBorderColorPropertyName;
					break;
				case PaintState::Focused:
					property_name = kFocusedBorderColorPropertyName;
					break;
				case PaintState::Disabled:
					property_name = kDisabledBorderColorPropertyName;
					break;
			}
			break;
	}

	if (property_name.empty()) {
		ZAFFAIL();
		return;
	}

	property_map_.SetProperty(property_name, color);
	NeedRepaint();
}


const std::wstring Control::GetText() const {
	return property_map_.GetProperty<std::wstring>(kTextPropertyName, []() { return std::wstring(); });
}


void Control::SetText(const std::wstring& text) {
	property_map_.SetProperty(kTextPropertyName, text);
	NeedRepaint();
}


TextAlignment Control::GetTextAlignment() const {
	return property_map_.GetProperty<TextAlignment>(
		kTextAlignmentPropertyName, 
		[]() { return TextAlignment::Leading; }
	);
}


void Control::SetTextAlignment(TextAlignment alignment) {
	property_map_.SetProperty(kTextAlignmentPropertyName, alignment);
	NeedRepaint();
}


ParagraphAlignment Control::GetParagraphAlignment() const {
	return property_map_.GetProperty<ParagraphAlignment>(
		kParagraphAlignmentPropertyName,
		[]() { return ParagraphAlignment::Near; }
	);
}


void Control::SetParagraphAlignment(ParagraphAlignment alignment) {
	property_map_.SetProperty(kParagraphAlignmentPropertyName, alignment);
	NeedRepaint();
}


WordWrapping Control::GetWordWrapping() const {
	return property_map_.GetProperty<WordWrapping>(
		kWordWrappingPropertyName, 
		[]() { return WordWrapping::NoWrap; }
	);
}


void Control::SetWordWrapping(WordWrapping word_wrapping) {
	property_map_.SetProperty(kWordWrappingPropertyName, word_wrapping);
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


const std::wstring Control::GetName() const {
	return property_map_.GetProperty<std::wstring>(kNamePropertyName, []() { return std::wstring(); });
}


void Control::SetName(const std::wstring& name) {
	property_map_.SetProperty(kNamePropertyName, name);
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

	ZAFLOG() << "IsFocusedChanged: " << is_focused;

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

}