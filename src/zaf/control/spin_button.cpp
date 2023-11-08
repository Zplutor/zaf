#include <zaf/control/spin_button.h>
#include <zaf/base/error/check.h>
#include <zaf/control/internal/triangle_geometry.h>
#include <zaf/graphic/canvas.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_TYPE(SpinButton)
ZAF_DEFINE_TYPE_END;

ArrowDirection SpinButton::Direction() const {
    return direction_;
}

void SpinButton::SetDirection(ArrowDirection direction) {

    ZAF_EXPECT(direction == ArrowDirection::Up || direction == ArrowDirection::Down);

    direction_ = direction;
    NeedRepaint();
}


ColorPicker SpinButton::ArrowColorPicker() const {

    if (arrow_color_picker_) {
        return arrow_color_picker_;
    }

    return [](const Control& control) {

        const auto& button = dynamic_cast<const SpinButton&>(control);

        if (!button.IsEnabledInContext()) {
            return Color::FromRGB(0xc0c0c0);
        }

        if (button.IsPressed()) {
            return Color::FromRGB(0x306DD9);
        }

        if (button.IsMouseOver()) {
            return Color::FromRGB(0x5080ef);
        }

        return Color::Black();
    };
}

void SpinButton::SetArrowColorPicker(ColorPicker picker) {
    arrow_color_picker_ = std::move(picker);
    NeedRepaint();
}


Color SpinButton::ArrowColor() const {
    return ArrowColorPicker()(*this);
}

void SpinButton::SetArrowColor(const Color& color) {
    SetArrowColorPicker(CreateColorPicker(color));
}


void SpinButton::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    auto width = Width();
    auto edge_length = width / 2;

    auto triangle_geometry = internal::CreateTriangleGeometry(
        width,
        Height(),
        edge_length,
        Direction() == ArrowDirection::Up ? 0.f : 180.f);

    auto state_guard = canvas.PushState();
    canvas.SetBrushWithColor(ArrowColor());
    canvas.DrawGeometry(triangle_geometry);
}



}