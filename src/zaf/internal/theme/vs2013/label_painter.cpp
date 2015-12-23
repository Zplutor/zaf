#include <zaf/internal/theme/vs2013/label_painter.h>
#include <zaf/canvas.h>
#include <zaf/control.h>
#include <zaf/label.h>
#include <zaf/internal/theme/vs2013/palette.h>

namespace zaf {
namespace internal {
namespace theme {
namespace vs2013 {

void LabelPainter::Paint(Canvas& canvas, const Rect& dirty_rect, const Control& control) {

	const auto& label = dynamic_cast<const Label&>(control);

	canvas.SetBrushColor(kControlBackgroundColor);
	canvas.DrawRectangle(Rect(label.GetSize()));

	canvas.SetBrushColor(label.IsEnabled() ? kTextColor : kDisabledTextColor);
	canvas.DrawSingleLineText(Rect(label.GetSize()), label.GetText(), kDefaultFont);
}

}
}
}
}