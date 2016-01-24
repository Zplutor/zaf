#include <zaf/control/label.h>

namespace zaf {

void Label::Paint(Canvas& canvas, const Rect& dirty_rect) {
	
	Control::Paint(canvas, dirty_rect);
	PaintText(canvas, dirty_rect, Rect(Point(), GetSize()));
}

}