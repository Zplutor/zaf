#include <zaf/control/check_box.h>
#include <zaf/base/assert.h>
#include <zaf/control/paint_utility.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/geometry/geometry_sink.h>
#include <zaf/graphic/geometry/path_geometry.h>

namespace zaf {

static const wchar_t* const kCanAutoChangeCheckStatePropertyName = L"CanAutoChangeCheckState";
static const wchar_t* const kCanBeIndeterminatePropertyName = L"CanBeIndeterminate";
static const wchar_t* const kDisabledBoxColorPropertyName = L"DisabledBoxColor";
static const wchar_t* const kFocusedBoxColorPropertyName = L"FocusedBoxColor";
static const wchar_t* const kHoveredBoxColorPropertyName = L"HoveredBoxColor";
static const wchar_t* const kNormalBoxColorPropertyName = L"NormalBoxColor";
static const wchar_t* const kPressedBoxColorPropertyName = L"PressedBoxColor";

CheckBox::CheckBox() : 
	check_state_(CheckState::Unchecked),
	OnCheckStateChange(check_state_change_event_) {

}


CheckBox::~CheckBox() {

}


void CheckBox::Initialize() {

	__super::Initialize();

	SetColor(PaintComponent::Box, PaintState::Normal, Color::Black);
	SetColor(PaintComponent::Box, PaintState::Hovered, Color::FromRGB(0x4169E1));
	SetColor(PaintComponent::Box, PaintState::Pressed, Color::FromRGB(0x0000CD));
	SetColor(PaintComponent::Box, PaintState::Disabled, Color::Gray);
}


void CheckBox::Paint(Canvas& canvas, const Rect& dirty_rect) {
	
	__super::Paint(canvas, dirty_rect);

	Canvas::StateGuard state_guard(canvas);

	auto text_layout = CreateTextLayout(Size());
	if (text_layout == nullptr) {
		return;
	}

	canvas.SetClearEdgeOption(ClearEdgeOption::Clear);

	PaintTextWithIcon(
		canvas,
		GetContentRect(), 
		text_layout, 
		12,
		[this](Canvas& canvas, const Rect& text_rect, const std::shared_ptr<TextLayout>& text_layout) {
			canvas.SetBrushWithColor(GetColor(PaintComponent::Foreground, GetPaintState()));
			canvas.DrawText(text_layout, text_rect.position);
		},
		std::bind(&CheckBox::PaintBox, this, std::placeholders::_1, std::placeholders::_2)
	);
}


void CheckBox::PaintBox(Canvas& canvas, const Rect& box_rect) const {

	//Paint the box frame.
	Color box_color = GetColor(PaintComponent::Box, GetPaintState());
	canvas.SetBrushWithColor(box_color);
	canvas.DrawRectangleFrame(box_rect, 1);

	//Paint the check state mark.
	auto check_state = GetCheckState();

	if (check_state == CheckState::Indeterminate) {
		Rect mark_rect = box_rect;
		mark_rect.Inflate(-3);
		canvas.DrawRectangle(mark_rect);
	}
	else if (check_state == CheckState::Checked) {

		auto path = canvas.CreatePathGeometry();
		if (path == nullptr) {
			return;
		}

		auto sink = path->Open();
		if (sink == nullptr) {
			return;
		}

		Rect mark_rect = box_rect;
		mark_rect.Inflate(-2);

		Point start_point(mark_rect.position.x + mark_rect.size.width, mark_rect.position.y);
		Point middle_point(mark_rect.position.x + mark_rect.size.width * 0.4f, mark_rect.position.y + mark_rect.size.height - 1);
		Point end_point(mark_rect.position.x, mark_rect.position.y + mark_rect.size.height * 0.4f);

		sink->BeginFigure(start_point, GeometrySink::BeginFigureOption::Hollow);
		sink->AddLine(middle_point);
		sink->AddLine(end_point);
		sink->EndFigure(GeometrySink::EndFigureOption::Open);
		sink->Close();

		canvas.DrawGeometryFrame(path, 1.5);
	}
}


const Color CheckBox::GetColor(int paint_component, int paint_state) const {

	if (paint_component == PaintComponent::Box) {

		return GetPropertyMap().GetProperty<Color>(
			GetBoxColorPropertyName(paint_state), 
			[this, paint_state]() {

				if (paint_state == PaintState::Normal) {
					return GetColor(PaintComponent::Foreground, PaintState::Normal);
				}
				else if (paint_state == PaintState::Pressed) {
					return GetColor(PaintComponent::Box, PaintState::Hovered);
				}
				else {
					return GetColor(PaintComponent::Box, PaintState::Normal);
				}
			}
		);
	}
	else {

		return __super::GetColor(paint_component, paint_state);
	}
}


void CheckBox::SetColor(int paint_component, int paint_state, const Color& new_color) {

	if (paint_component == PaintComponent::Box) {

		GetPropertyMap().SetProperty(GetBoxColorPropertyName(paint_state), new_color);
	}
	else {

		__super::SetColor(paint_component, paint_state, new_color);
	}
}


std::wstring CheckBox::GetBoxColorPropertyName(int paint_state) const {

	switch (paint_state) {

		case PaintState::Normal:
			return kNormalBoxColorPropertyName;

		case PaintState::Hovered:
			return kHoveredBoxColorPropertyName;

		case PaintState::Focused:
			return kFocusedBoxColorPropertyName;

		case PaintState::Pressed:
			return kPressedBoxColorPropertyName;

		case PaintState::Disabled:
			return kDisabledBoxColorPropertyName;

		default:
			ZAF_FAIL();
			return std::wstring();
	}
}


bool CheckBox::CanAutoChangeCheckState() const {

	return GetPropertyMap().GetProperty<bool>(
		kCanAutoChangeCheckStatePropertyName,
		[]() { return true; }
	);
}


void CheckBox::SetCanAutoChangeCheckState(bool can_change) {
	GetPropertyMap().SetProperty(kCanAutoChangeCheckStatePropertyName, can_change);
}


bool CheckBox::CanBeIndeterminate() const {

	return GetPropertyMap().GetProperty<bool>(
		kCanBeIndeterminatePropertyName,
		[]() { return false; }
	);
}


void CheckBox::SetCanBeIndeterminate(bool can_be_ndeterminate) {

	GetPropertyMap().SetProperty(kCanBeIndeterminatePropertyName, can_be_ndeterminate);

	if (! can_be_ndeterminate && (GetCheckState() == CheckState::Indeterminate)) {
		SetCheckState(CheckState::Checked);
	}
}


void CheckBox::SetCheckState(CheckState check_state) {

	if (check_state_ == check_state) {
		return;
	}

	check_state_ = check_state;
	NeedRepaint();

	check_state_change_event_.Trigger(std::dynamic_pointer_cast<CheckBox>(shared_from_this()));
}


void CheckBox::MouseClick() {

	if (! CanAutoChangeCheckState()) {
		return;
	}

	CheckState new_check_state = CheckState::Unchecked;

	switch (GetCheckState()) {

		case CheckState::Unchecked:
			new_check_state = CheckState::Checked;
			break;

		case CheckState::Checked:
			new_check_state = CanBeIndeterminate() ? CheckState::Indeterminate : CheckState::Unchecked;
			break;

		case CheckState::Indeterminate:
			new_check_state = CheckState::Unchecked;
			break;

		default:
			ZAF_FAIL();
			break;
	}

	SetCheckState(new_check_state);
}

}