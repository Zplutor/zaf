#include <zaf/internal/theme/vs2013/scroll_bar_painter.h>
#include <zaf/canvas.h>
#include <zaf/control.h>
#include <zaf/scroll_bar.h>
#include <zaf/internal/theme/vs2013/palette.h>

namespace zaf {
namespace internal {
namespace theme {
namespace vs2013 {

void ScrollBarPainter::Paint(Canvas& canvas, const Rect& dirty_rect, const Control& control) {

	canvas.SetBrushColor(kScrollBarBackgroundColor);
	canvas.DrawRectangle(Rect(control.GetSize()));
}


void ScrollBarArrowPainter::Paint(Canvas& canvas, const Rect& dirty_rect, const Control& control) {
	
	const Button& button = dynamic_cast<const Button&>(control);

	Rect arrow_block_rect(button.GetSize());

	canvas.SetBrushColor(kScrollBarBackgroundColor);
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

	canvas.SetBrushColor(color);
	canvas.DrawRectangle(arrow_rect);
}


void ScrollBarThumbPainter::Paint(Canvas& canvas, const Rect& dirty_rect, const Control& control) {

	const auto& thumb = dynamic_cast<const ScrollBar::Thumb&>(control);

	Rect slot_rect(thumb.GetSize());
	canvas.SetBrushColor(kScrollBarBackgroundColor);
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

	canvas.SetBrushColor(color);
	canvas.DrawRectangle(thumb_rect);
}

}
}
}
}