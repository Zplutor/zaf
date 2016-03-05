#include <zaf/control/button.h>

namespace zaf {

Button::Button() {

}


Button::~Button() {

}


void Button::Initialize() {

	__super::Initialize();

	SetBorderWidth(1);

	SetColor(PaintComponent::Background, Control::PaintState::Hovered, Color::FromRGB(0xF0F8FF));
	SetColor(PaintComponent::Background, PaintState::Pressed, Color::FromRGB(0xB0C4DE));
	SetColor(PaintComponent::Foreground, Control::PaintState::Disabled, Color::Gray);

	Color active_border_color = Color::FromRGB(0x4169E1);
	SetColor(PaintComponent::Border, Control::PaintState::Normal, Color::Black);
	SetColor(PaintComponent::Border, Control::PaintState::Hovered, active_border_color);
	SetColor(PaintComponent::Border, Control::PaintState::Focused, active_border_color);
	SetColor(PaintComponent::Border, PaintState::Pressed, active_border_color);
	SetColor(PaintComponent::Border, Control::PaintState::Disabled, Color::Gray);

	SetTextAlignment(TextAlignment::Center);
	SetParagraphAlignment(ParagraphAlignment::Center);
}


void Button::Paint(Canvas& canvas, const Rect& dirty_rect) {

	ClickableControl::Paint(canvas, dirty_rect);
	PaintText(canvas, dirty_rect, GetContentRect());
}

}