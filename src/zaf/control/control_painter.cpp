#include <zaf/control/control_painter.h>
#include <zaf/control/control.h>
#include <zaf/graphic/canvas.h>

namespace zaf {

const std::shared_ptr<ControlPainter>& ControlPainter::GetInstance() {

	static std::shared_ptr<ControlPainter> instance = std::make_shared<ControlPainter>();
	return instance;
}


void ControlPainter::Paint(Canvas& canvas, const Rect& dirty_rect, const Control& control) {

	Color background_color;
	Color border_color;

	if (! control.IsEnabled()) {

		background_color = control.GetDisabledBackgroundColor();
		border_color = control.GetDisabledBorderColor();
	}
	else if (control.IsHovered()) {

		background_color = control.GetHoveredBackgroundColor();
		border_color = control.GetHoveredBorderColor();
	}
	else if (control.IsFocused()) {

		background_color = control.GetFocusedBackgroundColor();
		border_color = control.GetFocusedBorderColor();
	}
	else {

		background_color = control.GetBackgroundColor();
		border_color = control.GetBorderColor();
	}

	Rect paint_rect(Point(), control.GetSize());
	canvas.SetBrushWithColor(border_color);
	canvas.DrawRectangle(paint_rect);

	paint_rect.Inflate(-control.GetBorderWidth());
	canvas.SetBrushWithColor(background_color);
	canvas.DrawRectangle(paint_rect);
}

}