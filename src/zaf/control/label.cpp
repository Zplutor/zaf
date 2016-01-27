#include <zaf/control/label.h>

namespace zaf {

Label::Label() {

	SetDisabledForegroundColor(Color::Gray);
}


Label::~Label() {

}


void Label::Paint(Canvas& canvas, const Rect& dirty_rect) {
	
	Control::Paint(canvas, dirty_rect);
	PaintText(canvas, dirty_rect, Rect(Point(), GetSize()));
}

}