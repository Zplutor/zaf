#include <zaf/control/check_box.h>
#include <zaf/internal/theme/painter.h>
#include <zaf/internal/theme/theme.h>

namespace zaf {

CheckBox::CheckBox() : 
	is_checked_(false) {

}


CheckBox::~CheckBox() {

}


void CheckBox::Paint(Canvas& canvas, const Rect& dirty_rect) {
	internal::theme::GetTheme()->GetCheckBoxPainter()->Paint(canvas, dirty_rect, *this);
}


void CheckBox::SetIsChecked(bool is_checked) {

	if (is_checked_ == is_checked) {
		return;
	}

	is_checked_ = is_checked;
	NeedRepaint();
}


void CheckBox::MouseClick() {
	is_checked_ = ! is_checked_;
}

}