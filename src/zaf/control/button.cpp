#include <zaf/control/button.h>

namespace zaf {

Button::Button() {

}


Button::~Button() {

}


void Button::Initialize() {

	__super::Initialize();

	SetBorderWidth(1);

	SetBackgroundColorPicker([](const Control& control) {
	
		const auto& button = dynamic_cast<const Button&>(control);

		if (button.IsPressed()) {
			return Color::FromRGB(0xB0C4DE);
		}

		if (button.IsHovered()) {
			return Color::FromRGB(0xF0F8FF);
		}

		return Color::White;
	});

	SetBorderColorPicker([](const Control& control) {
	
		const auto& button = dynamic_cast<const Button&>(control);

		if (! button.IsEnabled()) {
			return Color::Gray;
		}

		if (button.IsPressed() ||
			button.IsHovered() ||
			button.IsFocused()) {
			return Color::FromRGB(0x4169E1);
		}

		return Color::Black;
	});

	SetTextColorPicker([](const Control& control) {
		
		if (! control.IsEnabled()) {
			return Color::Gray;
		}

		return Color::Black;
	});

	SetTextAlignment(TextAlignment::Center);
	SetParagraphAlignment(ParagraphAlignment::Center);
}


}