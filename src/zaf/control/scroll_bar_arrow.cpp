#include <zaf/control/scroll_bar_arrow.h>
#include <zaf/control/internal/triangle_geometry.h>
#include <zaf/graphic/canvas.h>
#include <zaf/object/type_definition.h>
#include <zaf/serialization/properties.h>

namespace zaf {

ZAF_DEFINE_TYPE(ScrollBarArrow)
ZAF_DEFINE_TYPE_PROPERTY(ArrowColor)
ZAF_DEFINE_TYPE_PROPERTY(ArrowDirection)
ZAF_DEFINE_TYPE_END

ScrollBarArrow::ScrollBarArrow() : arrow_direction_(ArrowDirection::Up) {

}


void ScrollBarArrow::Initialize() {

    __super::Initialize();

    SetCanFocused(false);
}


void ScrollBarArrow::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) {

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


ColorPicker ScrollBarArrow::ArrowColorPicker() const {

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

            if (arrow.IsMouseOver()) {
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


void ScrollBarArrow::OnMouseCaptured(const MouseCapturedInfo& event_info) {

    __super::OnMouseCaptured(event_info);

    ScrollBarArrowBeginPressInfo begin_press_event_info{ As<ScrollBarArrow>(shared_from_this()) };
    begin_press_event_.GetObserver().OnNext(begin_press_event_info);
}


void ScrollBarArrow::OnMouseReleased(const MouseReleasedInfo& event_info) {

    __super::OnMouseReleased(event_info);

    ScrollBarArrowBeginPressInfo begin_press_event_info{ As<ScrollBarArrow>(shared_from_this()) };
    end_press_event_.GetObserver().OnNext(begin_press_event_info);
}

}