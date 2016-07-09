#include <zaf/control/paint_utility.h>
#include <zaf/application.h>
#include <zaf/control/paint_utility.h>
#include <zaf/control/textual_control.h>
#include <zaf/graphic/canvas.h>

namespace zaf {

void PaintTextWithIcon(
    Canvas& canvas,
    TextualControl& control,
    const std::shared_ptr<TextLayout>& text_layout,
    float icon_size,
    const std::function<void(Canvas& canvas, const Rect& icon_rect)>& paint_icon_function) {

    Rect text_rect = control.GetContentRect();
    float x_offset = icon_size + 5;
    text_rect.position.x += x_offset;
    text_rect.size.width -= x_offset;

    text_layout->SetMaxWidth(text_rect.size.width);
    text_layout->SetMaxHeight(text_rect.size.height);

    auto line_metrics = text_layout->GetLineMetrics(1);
    if (line_metrics.empty()) {
        return;
    }

    float icon_y = line_metrics.front().height - icon_size;
    if (icon_y != 0) {
        icon_y /= 2;
    }

    auto text_metrics = text_layout->GetMetrics();
    icon_y += text_metrics.top;

    Rect icon_rect(0, icon_y, icon_size, icon_size);
    paint_icon_function(canvas, icon_rect);

    canvas.SetBrushWithColor(control.GetTextColor());
    canvas.DrawText(text_layout, text_rect.position);
    
    if (control.IsFocused()) {
        Rect focus_rect(text_rect.position, Size(text_metrics.width, text_metrics.height));
        DrawFocusRectangleFrame(canvas, focus_rect);
    }
}


}