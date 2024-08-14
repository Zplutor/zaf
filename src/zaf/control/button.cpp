#include <zaf/control/button.h>
#include <zaf/creation.h>
#include <zaf/graphic/canvas.h>
#include <zaf/internal/theme.h>
#include <zaf/internal/paint_utility.h>

namespace zaf {
namespace {

const wchar_t* const kIsDefaultPropertyName = L"IsDefault";

}

ZAF_OBJECT_IMPL(Button);


Button::Button() {

}


Button::~Button() {

}


void Button::Initialize() {

    __super::Initialize();

    SetBorder(Frame(1));
    SetTextAlignment(TextAlignment::Center);
    SetParagraphAlignment(ParagraphAlignment::Center);

    SetBackgroundColorPicker(ColorPicker([](const Control& control) {

        const auto& button = As<Button>(control);

        if (!button.IsEnabledInContext()) {
            return Color::FromRGB(0xcccccc);
        }

        if (button.IsPressed()) {
            return Color::FromRGB(internal::ButtonPressedBackgroundColorRGB);
        }

        if (button.IsMouseOver()) {
            return Color::FromRGB(internal::ButtonMouseOverBackgroundColorRGB);
        }

        return Color::FromRGB(0xe1e1e1);
    }));

    SetBorderColorPicker(ColorPicker([](const Control& control) {

        const auto& button = As<Button>(control);

        if (!button.IsEnabledInContext()) {
            return Color::FromRGB(0xbfbfbf);
        }

        if (button.IsPressed() ||
            button.IsMouseOver() ||
            button.IsDefault()) {
            return Color::FromRGB(internal::ButtonActivedBorderColorRGB);
        }

        return Color::FromRGB(0xadadad);
    }));
}


void Button::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const {

    __super::Paint(canvas, dirty_rect);

    if (IsDefault()) {

        auto state_guard = canvas.PushState();
        canvas.SetBrushWithColor(Color::FromRGB(internal::ButtonActivedBorderColorRGB));
        canvas.DrawRectangleFrame(ContentRect(), 1);
    }
    
    if (IsFocused()) {
        auto focus_rect = ContentRect();
        focus_rect.Inflate(-1);
        internal::DrawFocusRectangleFrame(canvas, focus_rect);
    }
}


bool Button::IsDefault() const {
    return is_default_;
}


void Button::SetIsDefault(bool is_default) {

    is_default_ = is_default;
    NeedRepaint();
}


}