#include <zaf/control/check_box.h>
#include <zaf/base/assert.h>
#include <zaf/graphic/canvas.h>

namespace zaf {

static const wchar_t* const kNormalBoxColorPropertyName = L"NormalBoxColor";
static const wchar_t* const kHoveredBoxColorPropertyName = L"HoveredBoxColor";
static const wchar_t* const kFocusedBoxColorPropertyName = L"FocusedBoxColor";
static const wchar_t* const kPressedBoxColorPropertyName = L"PressedBoxColor";
static const wchar_t* const kDisabledBoxColorPropertyName = L"DisabledBoxColor";

CheckBox::CheckBox() : 
	is_checked_(false) {

	SetTextAlignment(TextAlignment::Leading);

	SetColor(PaintComponent::Box, PaintState::Normal, Color::Black);
	SetColor(PaintComponent::Box, PaintState::Hovered, Color::FromRGB(0x4169E1));
	SetColor(PaintComponent::Box, PaintState::Pressed, Color::FromRGB(0x0000CD));
	SetColor(PaintComponent::Box, PaintState::Disabled, Color::Gray);
}


CheckBox::~CheckBox() {

}


void CheckBox::Paint(Canvas& canvas, const Rect& dirty_rect) {
	
	__super::Paint(canvas, dirty_rect);

	const float kBoxSize = 12;
	const float kBoxMargin = 5;

	//Draw the box frame.
	Color box_color = GetColor(PaintComponent::Box, GetPaintState());

	Rect box_rect;
	box_rect.position.x = kBoxMargin + 0.5;
	box_rect.position.y = (GetHeight() - kBoxSize) / 2 + 0.5;
	box_rect.size.width = kBoxSize;
	box_rect.size.height = kBoxSize;

	canvas.SetBrushWithColor(box_color);
	canvas.DrawRectangleFrame(box_rect, 1);

	//Draw the checked mark.
	if (IsChecked()) {
		box_rect.Inflate(-2.5);
		canvas.DrawRectangle(box_rect);
	}

	//Draw text.
	Rect text_rect = GetContentRect();
	text_rect.position.x += kBoxSize + kBoxMargin * 2;
	text_rect.size.width -= kBoxSize + kBoxMargin * 2;
	PaintText(canvas, dirty_rect, text_rect);
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


void CheckBox::SetIsChecked(bool is_checked) {

	if (is_checked_ == is_checked) {
		return;
	}

	is_checked_ = is_checked;
	NeedRepaint();
}


void CheckBox::MouseClick() {
	is_checked_ = ! is_checked_;
}

}