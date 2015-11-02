#include <zaf/internal/theme/vs2013/single_color_painter.h>
#include <zaf/canvas.h>
#include <zaf/control.h>
#include <zaf/internal/theme/vs2013/palette.h>

namespace zaf {
namespace internal {
namespace theme {
namespace vs2013 {

void SingleColorPainter::Paint(Canvas& canvas, const Rect& dirty_rect, const Control& control) {

	canvas.SetBrushColor(color_);
	canvas.DrawRectangle(Rect(Point(), control.GetRect().size));
}

}
}
}
}