#include <zaf/control/button.h>

namespace zaf {

Button::Button() {

	SetBorderWidth(1);

	SetHoveredBackgroundColor(Color::FromRGB(0xF0F8FF));
	SetPressedBackgroundColor(Color::FromRGB(0xB0C4DE));
	SetDisabledForegroundColor(Color::Gray);

	Color active_border_color = Color::FromRGB(0x4169E1);

	SetBorderColor(Color::Black);
	SetHoveredBorderColor(active_border_color);
	SetFocusedBorderColor(active_border_color);
	SetPressedBorderColor(active_border_color);
	SetDisabledBorderColor(Color::Gray);
}


Button::~Button() {

}


void Button::Paint(Canvas& canvas, const Rect& dirty_rect) {

	ClickableControl::Paint(canvas, dirty_rect);
	PaintText(canvas, dirty_rect, Rect(Point(), GetSize()));
}

}