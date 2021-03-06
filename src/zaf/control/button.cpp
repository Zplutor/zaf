#include <zaf/control/button.h>
#include <zaf/creation.h>
#include <zaf/graphic/canvas.h>
#include <zaf/internal/theme.h>
#include <zaf/internal/paint_utility.h>
#include <zaf/parsing/parsers/button_parser.h>
#include <zaf/reflection/reflection_type.h>
#include <zaf/reflection/reflection_type_definition.h>

namespace zaf {
namespace {

const wchar_t* const kIsDefaultPropertyName = L"IsDefault";

}

ZAF_DEFINE_REFLECTION_TYPE(Button)
    ZAF_DEFINE_PARSER(ButtonParser)
ZAF_DEFINE_END


Button::Button() {

}


Button::~Button() {

}


void Button::Initialize() {

	__super::Initialize();

	SetBorder(1);

	SetBackgroundColorPicker([](const Control& control) {
	
		const auto& button = dynamic_cast<const Button&>(control);

        if (! button.IsEnabled()) {
            return Color::FromRGB(0xcccccc);
        }

		if (button.IsPressed()) {
			return Color::FromRGB(internal::ButtonPressedBackgroundColorRGB);
		}

		if (button.IsHovered()) {
			return Color::FromRGB(internal::ButtonHoveredBackgroundColorRGB);
		}

        return Color::FromRGB(0xe1e1e1);
	});

	SetBorderColorPicker([](const Control& control) {
	
		const auto& button = dynamic_cast<const Button&>(control);

		if (! button.IsEnabled()) {
            return Color::FromRGB(0xbfbfbf);
		}

		if (button.IsPressed() ||
			button.IsHovered() || 
            button.IsDefault()) {
			return Color::FromRGB(internal::ButtonActivedBorderColorRGB);
		}

		return Color::FromRGB(0xadadad);
	});

	SetTextAlignment(TextAlignment::Center);
	SetParagraphAlignment(ParagraphAlignment::Center);
}


void Button::Paint(Canvas& canvas, const Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    if (IsDefault()) {

        Canvas::StateGuard state_guard(canvas);
        canvas.SetBrushWithColor(Color::FromRGB(internal::ButtonActivedBorderColorRGB));
        canvas.DrawRectangleFrame(GetContentRect(), 1);
    }
    
    if (IsFocused()) {
        auto focus_rect = GetContentRect();
        focus_rect.Inflate(-1);
        internal::DrawFocusRectangleFrame(canvas, focus_rect);
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