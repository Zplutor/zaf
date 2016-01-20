#include <zaf/control/painter.h>
#include <zaf/control/control.h>
#include <zaf/graphic/canvas.h>

namespace zaf {

void Painter::Paint(Canvas& canvas, const Rect& dirty_rect, const std::shared_ptr<const Control>& control) {

	Rect rect = Rect(Point(), control->GetSize());
	canvas.SetBrushWithColor(control->GetBorderColor());
	canvas.DrawRectangle(rect);


}

}