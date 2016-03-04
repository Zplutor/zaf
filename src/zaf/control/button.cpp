#include <zaf/control/button.h>
#include <zaf/control/text_properties.h>

namespace zaf {

Button::Button() {

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

	TextProperties text_properties;
	text_properties.text_alignment = TextAlignment::Center;
	text_properties.paragraph_alignment = ParagraphAlignment::Center;
}


Button::~Button() {

}


void Button::Paint(Canvas& canvas, const Rect& dirty_rect) {

	ClickableControl::Paint(canvas, dirty_rect);
	PaintText(canvas, dirty_rect, GetContentRect());
}

}