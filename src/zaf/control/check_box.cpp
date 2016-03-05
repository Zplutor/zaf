#include <zaf/control/check_box.h>
#include <zaf/base/assert.h>
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

	SetColor(PaintComponent::Box, PaintState::Normal, Color::Black);
	SetColor(PaintComponent::Box, PaintState::Hovered, Color::FromRGB(0x4169E1));
	SetColor(PaintComponent::Box, PaintState::Pressed, Color::FromRGB(0x0000CD));
	SetColor(PaintComponent::Box, PaintState::Disabled, Color::Gray);
}


CheckBox::~CheckBox() {

}


void CheckBox::Paint(Canvas& canvas, const Rect& dirty_rect) {
	
	__super::Paint(canvas, dirty_rect);

	const float box_size = 12;
	const float box_margin = 5;

	//Paint box.
	Rect box_rect;
	box_rect.position.x = box_margin + 0.5f;
	box_rect.size.width = box_size;
	box_rect.size.height = box_size;

	switch (GetParagraphAlignment()) {
		case ParagraphAlignment::Near:
			box_rect.position.y = box_margin + 0.5f;
			break;
		case ParagraphAlignment::Center:
			box_rect.position.y = (GetHeight() - box_size) / 2 + 0.5f;
			break;
		case ParagraphAlignment::Far:
			box_rect.position.y = GetHeight() - box_size - box_margin + 0.5f;
			break;
	}

	PaintBox(canvas, box_rect);

	//Paint text.
	Rect text_rect = GetContentRect();
	text_rect.position.x += box_size + box_margin * 2;
	text_rect.size.width -= box_size + box_margin * 2;
	PaintText(canvas, dirty_rect, text_rect);
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
		mark_rect.Inflate(-2.5);
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
		Point middle_point(mark_rect.position.x + mark_rect.size.width * 0.4, mark_rect.position.y + mark_rect.size.height - 1);
		Point end_point(mark_rect.position.x, mark_rect.position.y + mark_rect.size.height * 0.4);

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
			ZAFFAIL();
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
			ZAFFAIL();
			break;
	}

	SetCheckState(new_check_state);
}

}