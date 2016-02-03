#include <zaf/internal/theme/vs2013/check_box_painter.h>
#include <zaf/control/check_box.h>
#include <zaf/graphic/canvas.h>
#include <zaf/internal/theme/vs2013/palette.h>

namespace zaf {
namespace internal {
namespace theme {
namespace vs2013 {

static const float kBoxFrameSize = 12;

void CheckBoxPainter::Paint(Canvas& canvas, const Rect& dirty_rect, const Control& control) {

	const auto& check_box = dynamic_cast<const CheckBox&>(control);

	Rect frame_rect;
	frame_rect.position.x = 0;
	frame_rect.position.y = (control.GetSize().height - kBoxFrameSize) / 2;
	frame_rect.size.width = kBoxFrameSize;
	frame_rect.size.height = kBoxFrameSize;
	canvas.SetBrushWithColor(check_box.IsHovered() ? kCheckBoxHoveredFrameColor : kCheckBoxFrameColor);
	canvas.DrawRectangleFrame(frame_rect, 1);

	if (check_box.IsChecked()) {
		frame_rect.Inflate(-2, -2);
		canvas.SetBrushWithColor(Color::Black);
		canvas.DrawRectangle(frame_rect);
	}

	Rect text_rect(Point(), check_box.GetSize());
	text_rect.Inflate(-kBoxFrameSize, 0);
	canvas.SetBrushWithColor(Color::Black);
}

}
}
}
}