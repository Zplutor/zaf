#include <zaf/internal/theme/vs2013/button_painter.h>
#include <zaf/control/button.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/text/font.h>
#include <zaf/internal/theme/vs2013/palette.h>

namespace zaf {
namespace internal {
namespace theme {
namespace vs2013 {

void ButtonPainter::Paint(Canvas& canvas, const Rect& dirty_rect, const Control& control) {

	const Button& button = dynamic_cast<const Button&>(control);

	canvas.SetBrushWithColor(GetBackgroundColor(button));
	canvas.DrawRectangle(Rect(Point(), button.GetRect().size));

	canvas.SetBrushWithColor(GetBorderColor(button));
	canvas.DrawRectangleFrame(Rect(Point(), button.GetRect().size), 1);

	canvas.SetBrushWithColor(button.IsEnabled() ? kTextColor : kDisabledTextColor);
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