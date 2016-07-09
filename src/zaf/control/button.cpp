#include <zaf/control/button.h>
#include <zaf/control/paint_utility.h>
#include <zaf/internal/theme.h>

namespace zaf {

static const wchar_t* const kIsDefaultPropertyName = L"IsDefault";

Button::Button() {

}


Button::~Button() {

}


void Button::Initialize() {

	__super::Initialize();

	SetBorderThickness(1);

	SetBackgroundColorPicker([](const Control& control) {
	
		const auto& button = dynamic_cast<const Button&>(control);

        if (! button.IsEnabled()) {
            return internal::ButtonDisabledBackgroundColor;
        }

		if (button.IsPressed()) {
			return internal::ButtonPressedBackgroundColor;
		}

		if (button.IsHovered()) {
			return internal::ButtonHoveredBackgroundColor;
		}

		return internal::ButtonNormalBackgroundColor;
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

	SetTextAlignment(TextAlignment::Center);
	SetParagraphAlignment(ParagraphAlignment::Center);
}


void Button::Paint(Canvas& canvas, const Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    if (IsFocused()) {
        auto focus_rect = GetContentRect();
        focus_rect.Inflate(-1);
        DrawFocusRectangleFrame(canvas, focus_rect);
    }
}


bool Button::IsDefault() const {

    auto is_default = GetPropertyMap().TryGetProperty<bool>(kIsDefaultPropertyName);
    if (is_default != nullptr) {
        return *is_default;
    }
    else {
        return false;
    }
}


void Button::SetIsDefault(bool is_default) {

    GetPropertyMap().SetProperty(kIsDefaultPropertyName, is_default);
    NeedRepaint();
}


}