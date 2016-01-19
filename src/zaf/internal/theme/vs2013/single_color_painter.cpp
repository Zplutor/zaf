#include <zaf/internal/theme/vs2013/single_color_painter.h>
#include <zaf/control/control.h>
#include <zaf/graphic/canvas.h>
#include <zaf/internal/theme/vs2013/palette.h>

namespace zaf {
namespace internal {
namespace theme {
namespace vs2013 {

void SingleColorPainter::Paint(Canvas& canvas, const Rect& dirty_rect, const Control& control) {

	canvas.SetBrushWithColor(color_);
	canvas.DrawRectangle(Rect(Point(), control.GetRect().size));
}

}
}
}
}