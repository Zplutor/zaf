#include <zaf/control/label.h>

namespace zaf {

Label::Label() {

	SetColor(PaintComponent::Foreground, PaintState::Disabled, Color::Gray);
}


Label::~Label() {

}


void Label::Paint(Canvas& canvas, const Rect& dirty_rect) {
	
	__super::Paint(canvas, dirty_rect);
	PaintText(canvas, dirty_rect, Rect(Point(), GetSize()));
}

}