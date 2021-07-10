#include <zaf/control/scroll_bar_arrow.h>
#include <zaf/control/internal/triangle_geometry.h>
#include <zaf/graphic/canvas.h>
#include <zaf/parsing/parsers/scroll_bar_arrow_parser.h>
#include <zaf/object/type_definition.h>
#include <zaf/serialization/properties.h>

namespace zaf {

ZAF_DEFINE_TYPE(ScrollBarArrow)
    ZAF_DEFINE_TYPE_PARSER(ScrollBarArrowParser)
ZAF_DEFINE_TYPE_END

ScrollBarArrow::ScrollBarArrow() : direction_(Direction::Up) {

}


void ScrollBarArrow::Initialize() {

    __super::Initialize();

    SetCanFocused(false);
}


void ScrollBarArrow::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    Direction direction = GetDirection();

    float bottom_edge_length =
        (direction == Direction::Up || direction == Direction::Down) ? Width() : Height();

    bottom_edge_length /= 2;

    float rotate_angle = 0;
    switch (direction) {
    case Direction::Left:
        rotate_angle = 270;
        break;
    case Direction::Right:
        rotate_angle = 90;
        break;
    case Direction::Down:
        rotate_angle = 180;
        break;
    default:
        break;
    }

    auto triangle_geometry = internal::CreateTriangleGeometry(
        Width(),
        Height(),
        bottom_edge_length,
        rotate_angle);

    Canvas::StateGuard state_guard(canvas);
    canvas.SetBrushWithColor(GetArrowColor());
    canvas.DrawGeometry(triangle_geometry);
}


ColorPicker ScrollBarArrow::GetArrowColorPicker() const {

    auto color_picker = GetPropertyMap().TryGetProperty<ColorPicker>(property::ArrowColorPicker);
    if ((color_picker != nullptr) && (*color_picker != nullptr)) {
        return *color_picker;
    }
    else {

        return [](const Control& control) {

            const auto& arrow = dynamic_cast<const ScrollBarArrow&>(control);

            if (!arrow.IsEnabled()) {
                return Color::FromRGB(0xc0c0c0);
            }

            if (arrow.IsPressed()) {
                return Color::FromRGB(0x306DD9);
            }

            if (arrow.IsHovered()) {
                return Color::FromRGB(0x5080ef);
            }

            return Color::FromRGB(0x808080);
        };
    }
}


void ScrollBarArrow::SetArrowColorPicker(const ColorPicker& color_picker) {

    GetPropertyMap().SetProperty(property::ArrowColorPicker, color_picker);
    NeedRepaint();
}


void ScrollBarArrow::OnMouseCapture() {

    ClickableControl::OnMouseCapture();

    ScrollBarArrowBeginPressInfo event_info;
    event_info.scroll_bar_arrow = std::dynamic_pointer_cast<ScrollBarArrow>(shared_from_this());
    begin_press_event_.GetObserver().OnNext(event_info);
}


void ScrollBarArrow::OnMouseRelease() {

    ClickableControl::OnMouseRelease();

    ScrollBarArrowEndPressInfo event_info;
    event_info.scroll_bar_arrow = std::dynamic_pointer_cast<ScrollBarArrow>(shared_from_this());
    end_press_event_.GetObserver().OnNext(event_info);
}

}