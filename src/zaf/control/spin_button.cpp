#include <zaf/control/spin_button.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/control/internal/triangle_geometry.h>
#include <zaf/graphic/canvas.h>

namespace zaf {

ZAF_OBJECT_IMPL(SpinButton);

void SpinButton::Initialize() {

    __super::Initialize();

    SetArrowColorPicker(ColorPicker([](const Control& control) {

        const auto& button = As<SpinButton>(control);

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
    }));
}


ArrowDirection SpinButton::Direction() const {
    return direction_;
}

void SpinButton::SetDirection(ArrowDirection direction) {

    ZAF_EXPECT(direction == ArrowDirection::Up || direction == ArrowDirection::Down);

    direction_ = direction;
    NeedRepaint();
}


Color SpinButton::ArrowColor() const {
    return arrow_color_field_.Color();
}

void SpinButton::SetArrowColor(const Color& color) {
    arrow_color_field_.SetColor(color);
}


const ColorPicker& SpinButton::ArrowColorPicker() const {
    return arrow_color_field_.ColorPicker();
}

void SpinButton::SetArrowColorPicker(ColorPicker picker) {
    arrow_color_field_.SetColorPicker(std::move(picker));
}

void SpinButton::UpdateStyle() {

    __super::UpdateStyle();

    arrow_color_field_.UpdateColor();
}


void SpinButton::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const {

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