#include <zaf/control/spin_button.h>
#include <zaf/base/error/contract_error.h>
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


Color SpinButton::ArrowColor() const {
    return arrow_color_;
}

void SpinButton::SetArrowColor(const Color& color) {
    arrow_color_ = color;
    NeedRepaint();
}


void SpinButton::UpdateStyle() {

    __super::UpdateStyle();

    SetArrowColor([this]() {
    
        if (!IsEnabledInContext()) {
            return Color::FromRGB(0xc0c0c0);
        }

        if (IsPressed()) {
            return Color::FromRGB(0x306DD9);
        }

        if (IsMouseOver()) {
            return Color::FromRGB(0x5080ef);
        }

        return Color::Black();
    }());
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