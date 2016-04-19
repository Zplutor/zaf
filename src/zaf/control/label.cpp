#include <zaf/control/label.h>
#include <zaf/graphic/canvas.h>

namespace zaf {

Label::Label() {

}


Label::~Label() {

}


void Label::Initialize() {

	__super::Initialize();

	SetTextColorPicker([](const Control& control) {

		if (control.IsEnabled()) {
			return Color::Black;
		}
		else {
			return Color::Gray;
		}
	});
}


void Label::Paint(Canvas& canvas, const Rect& dirty_rect) {
	
	__super::Paint(canvas, dirty_rect);

	Canvas::StateGuard state_guard(canvas);

	Rect content_rect = GetContentRect();

	auto text_layout = CreateTextLayout(content_rect.size);
	if (text_layout == nullptr) {
		return;
	}

	canvas.SetBrushWithColor(GetTextColor());
	canvas.DrawText(text_layout, content_rect.position);
}

}