#include <zaf/internal/theme/vs2013/scroll_bar_painter.h>
#include <zaf/control/control.h>
#include <zaf/control/scroll_bar.h>
#include <zaf/graphic/canvas.h>
#include <zaf/internal/theme/vs2013/palette.h>

namespace zaf {
namespace internal {
namespace theme {
namespace vs2013 {

void ScrollBarPainter::Paint(Canvas& canvas, const Rect& dirty_rect, const Control& control) {

	canvas.SetBrushWithColor(kScrollBarBackgroundColor);
	canvas.DrawRectangle(Rect(Point(), control.GetSize()));
}


void ScrollBarArrowPainter::Paint(Canvas& canvas, const Rect& dirty_rect, const Control& control) {
	
	const ClickableControl& button = dynamic_cast<const ClickableControl&>(control);

	Rect arrow_block_rect(Point(), button.GetSize());

	canvas.SetBrushWithColor(kScrollBarBackgroundColor);
	canvas.DrawRectangle(arrow_block_rect);

	Rect arrow_rect(arrow_block_rect);
	arrow_rect.position.x += 4;
	arrow_rect.position.y += 4;
	arrow_rect.size.width -= 8;
	arrow_rect.size.height -= 8;

	Color color(kScrollBarArrowColor);
	if (button.IsHovered()) {
		color = kScrollBarArrowHoveredColor;
	}

	if (button.IsPressed()) {
		color = kScrollBarArrowPressedColor;
	}

	canvas.SetBrushWithColor(color);
	canvas.DrawRectangle(arrow_rect);
}


void ScrollBarThumbPainter::Paint(Canvas& canvas, const Rect& dirty_rect, const Control& control) {

	const auto& thumb = dynamic_cast<const ScrollBar::Thumb&>(control);

	Rect slot_rect(Point(), thumb.GetSize());
	canvas.SetBrushWithColor(kScrollBarBackgroundColor);
	canvas.DrawRectangle(slot_rect);

	Rect thumb_rect = slot_rect;
	if (thumb.IsHorizontal()) {
		thumb_rect.position.y += 4;
		thumb_rect.size.height -= 8;
	}
	else {
		thumb_rect.position.x += 4;
		thumb_rect.size.width -= 8;
	}

	Color color(kScrollBarThumbColor);
	if (thumb.IsHovered()) {
		color = kScrollBarThumbHoveredColor;
	}

	if (thumb.IsDragging()) {
		color = kScrollBarThumbPressedColor;
	}

	canvas.SetBrushWithColor(color);
	canvas.DrawRectangle(thumb_rect);
}

}
}
}
}