#include <zaf/internal/theme/vs2013/label_painter.h>
#include <zaf/canvas.h>
#include <zaf/control.h>
#include <zaf/internal/theme/vs2013/palette.h>

namespace zaf {
namespace internal {
namespace theme {
namespace vs2013 {

void LabelPainter::Paint(Canvas& canvas, const Rect& dirty_rect, const Control& control) {

	canvas.SetBrushColor(kControlBackgroundColor);
	canvas.DrawRectangle(Rect(control.GetSize()));

	canvas.SetBrushColor(control.IsEnabled() ? kTextColor : kDisabledTextColor);
	canvas.DrawSingleLineText(Rect(control.GetSize()), control.GetText(), kDefaultFont);
}

}
}
}
}