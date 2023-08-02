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
    canvas.SetBrushWithColor(zaf::Color::Black());
    canvas.DrawGeometry(triangle_geometry);
}



}