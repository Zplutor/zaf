#include <zaf/control/label.h>

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


}