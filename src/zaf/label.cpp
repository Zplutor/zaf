#include <zaf/label.h>
#include <zaf/internal/theme/painter.h>
#include <zaf/internal/theme/theme.h>

namespace zaf {

void Label::Paint(Canvas& canvas, const Rect& dirty_rect) {
	internal::theme::GetTheme()->GetLabelPainter()->Paint(canvas, dirty_rect, *this);
}

}