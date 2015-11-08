#include <zaf/button.h>
#include <zaf/internal/theme/painter.h>
#include <zaf/internal/theme/theme.h>

namespace zaf {

Button::Button() {

}


Button::~Button() {

}


void Button::Paint(Canvas& canvas, const Rect& dirty_rect) {
	internal::theme::GetTheme()->GetButtonPainter()->Paint(canvas, dirty_rect, *this);
}


}