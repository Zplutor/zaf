#include <zaf/internal/theme/vs2013/button_painter.h>
#include <zaf/button.h>
#include <zaf/canvas.h>
#include <zaf/font.h>
#include <zaf/internal/theme/vs2013/palette.h>

namespace zaf {
namespace internal {
namespace theme {
namespace vs2013 {

void ButtonPainter::Paint(Canvas& canvas, const Rect& dirty_rect, const Control& control) {

	const Button& button = dynamic_cast<const Button&>(control);

	canvas.SetBrushColor(GetBackgroundColor(button));
	canvas.DrawRectangle(Rect(Point(), control.GetRect().size));

	canvas.SetBrushColor(GetBorderColor(button));
	canvas.DrawRectangleFrame(Rect(Point(), control.GetRect().size), 1);

	canvas.SetBrushColor(control.IsEnabled() ? kTextColor : kDisabledTextColor);
	canvas.DrawSingleLineText(Rect(Point(), control.GetRect().size), control.GetText(), kDefaultFont);
}


const Color& ButtonPainter::GetBackgroundColor(const Button& button) {

	if (button.IsHovered() && button.IsPressed()) {
		return kButtonHoveredPressedBackgroundColor;
	}

	if (button.IsHovered() ^ button.IsPressed()) {
		return kButtonHoveredBackgroundColor;
	}

	return kControlBackgroundColor;
}


const Color& ButtonPainter::GetBorderColor(const Button& button) {

	if (button.IsHovered() || button.IsPressed()) {
		return kButtonHoveredBorderColor;
	}

	return kControlBackgroundColor;
}

}
}
}
}