#include <zaf/control/label.h>
#include <zaf/graphic/canvas.h>

namespace zaf {

Label::Label() {

}


Label::~Label() {

}


void Label::Initialize() {
	__super::Initialize();
	SetColor(PaintComponent::Foreground, PaintState::Disabled, Color::Gray);
}


void Label::Paint(Canvas& canvas, const Rect& dirty_rect) {
	
	__super::Paint(canvas, dirty_rect);

	Canvas::StateGuard state_guard(canvas);
	PaintText(canvas, dirty_rect, Rect(Point(), GetSize()));
}

}