#include <zaf/control/paint_utility.h>
#include <zaf/application.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/stroke.h>
#include <zaf/graphic/stroke_properties.h>

namespace zaf {

void DrawFocusRectangleFrame(Canvas& canvas, const Rect& rect) {

    StrokeProperties stroke_properties;
    stroke_properties.dash_style = Stroke::DashStyle::Dot;
    stroke_properties.dash_cap_style = Stroke::CapStyle::Square;
    auto stroke = GetRendererFactory()->CreateStroke(stroke_properties);
    if (stroke == nullptr) {
        return;
    }

    canvas.SetStroke(stroke);
    canvas.SetBrushWithColor(Color::Black);
    canvas.DrawRectangleFrame(rect, 1);
}

}