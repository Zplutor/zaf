#include <zaf/internal/paint_utility.h>
#include <zaf/application.h>
#include <zaf/control/clickable_control.h>
#include <zaf/control/textual_control.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/resource_factory.h>
#include <zaf/graphic/stroke_properties.h>
#include <zaf/internal/theme.h>

namespace zaf {
namespace internal {

void DrawTextWithIcon(
    Canvas& canvas,
    TextualControl& control,
    TextLayout& text_layout,
    float icon_size,
    const std::function<void(Canvas& canvas, const Rect& icon_rect)>& paint_icon_function) {

    Rect text_rect = control.GetContentRect();
    float x_offset = icon_size + 5;
    text_rect.position.x += x_offset;
    text_rect.size.width -= x_offset;

    text_layout.SetMaxWidth(text_rect.size.width);
    text_layout.SetMaxHeight(text_rect.size.height);

    auto line_metrics = text_layout.GetLineMetrics(1);
    if (line_metrics.empty()) {
        return;
    }

    float icon_y = line_metrics.front().height - icon_size;
    if (icon_y != 0) {
        icon_y /= 2;
    }

    auto text_metrics = text_layout.GetMetrics();
    icon_y += text_metrics.top;

    Rect icon_rect(0, icon_y, icon_size, icon_size);
    paint_icon_function(canvas, icon_rect);

    canvas.SetBrushWithColor(control.GetTextColor());
    canvas.DrawTextLayout(text_layout, text_rect.position);
    
    if (control.IsFocused()) {

        Rect focus_rect;
        focus_rect.position.x = text_rect.position.x + text_metrics.left;
        focus_rect.position.y = text_rect.position.y + text_metrics.top;
        focus_rect.size = Size{ text_metrics.width, text_metrics.height };
        DrawFocusRectangleFrame(canvas, focus_rect);
    }
}


void DrawFocusRectangleFrame(Canvas& canvas, const Rect& rect) {

    StrokeProperties stroke_properties;
    stroke_properties.dash_style = Stroke::DashStyle::Dot;
    stroke_properties.dash_cap_style = Stroke::CapStyle::Square;
    auto stroke = GetGraphicFactory().CreateStroke(stroke_properties);

    canvas.SetStroke(stroke);
    canvas.SetBrushWithColor(Color::Black());
    canvas.DrawRectangleFrame(rect, 1);
}


const ColorPicker GetBoxBackgroundColorPicker() {

    return [](const Control& control) {

        const auto& clickable_control = dynamic_cast<const ClickableControl&>(control);

        if (!clickable_control.IsEnabled()) {
            return Color::FromRGB(internal::ControlBackgroundColorRGB);
        }

        if (clickable_control.IsPressed()) {
            return Color::FromRGB(internal::ButtonPressedBackgroundColorRGB);;
        }

        if (clickable_control.IsHovered()) {
            return Color::FromRGB(internal::ButtonHoveredBackgroundColorRGB);
        }

        return Color::FromRGB(internal::ControlContentColorRGB);
    };
}


const ColorPicker GetBoxBorderColorPicker() {

    return [](const Control& control) {

        const auto& clickable_control = dynamic_cast<const ClickableControl&>(control);

        if (!clickable_control.IsEnabled()) {
            return Color::Gray();
        }

        if (clickable_control.IsPressed() ||
            clickable_control.IsHovered()) {
            return Color::FromRGB(internal::ButtonActivedBorderColorRGB);
        }

        return Color::Black();
    };
}


}
}