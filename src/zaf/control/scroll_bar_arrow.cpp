#include <zaf/control/scroll_bar_arrow.h>
#include <zaf/control/internal/triangle_geometry.h>
#include <zaf/graphic/canvas.h>

namespace zaf {

ZAF_OBJECT_IMPL(ScrollBarArrow);

ScrollBarArrow::ScrollBarArrow() : arrow_direction_(ArrowDirection::Up) {

}


void ScrollBarArrow::Initialize() {

    __super::Initialize();

    SetCanFocused(false);

    SetArrowColorPicker(ColorPicker([](const Control& control) {

        const auto& arrow = As<ScrollBarArrow>(control);

        if (!arrow.IsEnabledInContext()) {
            return Color::FromRGB(0xc0c0c0);
        }

        if (arrow.IsPressed()) {
            return Color::FromRGB(0x306DD9);
        }

        if (arrow.IsMouseOver()) {
            return Color::FromRGB(0x5080ef);
        }

        return Color::FromRGB(0x808080);
    }));
}


void ScrollBarArrow::UpdateStyle() {

    __super::UpdateStyle();

    arrow_color_field_.UpdateColor();
}


void ScrollBarArrow::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const {

    __super::Paint(canvas, dirty_rect);

    zaf::ArrowDirection direction = ArrowDirection();

    float bottom_edge_length =
        (direction == zaf::ArrowDirection::Up || direction == zaf::ArrowDirection::Down) ? 
        Width() :
        Height();

    bottom_edge_length /= 2;

    float rotate_angle = 0;
    switch (direction) {
    case zaf::ArrowDirection::Left:
        rotate_angle = 270;
        break;
    case zaf::ArrowDirection::Right:
        rotate_angle = 90;
        break;
    case zaf::ArrowDirection::Down:
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

    auto state_guard = canvas.PushState();
    canvas.SetBrushWithColor(ArrowColor());
    canvas.DrawGeometry(triangle_geometry);
}


Color ScrollBarArrow::ArrowColor() const {
    return arrow_color_field_.Color();
}

void ScrollBarArrow::SetArrowColor(const Color& color) {
    arrow_color_field_.SetColor(color);
}


const ColorPicker& ScrollBarArrow::ArrowColorPicker() const {
    return arrow_color_field_.ColorPicker();
}

void ScrollBarArrow::SetArrowColorPicker(ColorPicker color_picker) {
    arrow_color_field_.SetColorPicker(std::move(color_picker));
}


void ScrollBarArrow::OnMouseCaptured(const MouseCapturedInfo& event_info) {

    __super::OnMouseCaptured(event_info);

    ScrollBarArrowBeginPressInfo begin_press_event_info{ As<ScrollBarArrow>(shared_from_this()) };
    begin_press_event_.AsObserver().OnNext(begin_press_event_info);
}


void ScrollBarArrow::OnMouseReleased(const MouseReleasedInfo& event_info) {

    __super::OnMouseReleased(event_info);

    ScrollBarArrowBeginPressInfo begin_press_event_info{ As<ScrollBarArrow>(shared_from_this()) };
    end_press_event_.AsObserver().OnNext(begin_press_event_info);
}

}