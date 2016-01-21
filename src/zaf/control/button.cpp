#include <zaf/control/button.h>

namespace zaf {

Button::Button() {

	SetBorderWidth(1);

	SetHoveredBackgroundColor(Color::FromRGB(0xFDF4BF));
	SetHoveredBorderColor(Color::FromRGB(0xE5C365));

	SetPressedBackgroundColor(Color::FromRGB(0xFFF29D));
}


Button::~Button() {

}


}