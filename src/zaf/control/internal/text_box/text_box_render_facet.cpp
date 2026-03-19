#include <zaf/control/internal/text_box/text_box_render_facet.h>
#include <zaf/control/internal/textual/text_box_editor.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/d2d/stroke_properties.h>
#include <zaf/graphic/graphic_factory.h>

namespace zaf::internal {

TextBoxRenderFacet::TextBoxRenderFacet(TextBox& owner) noexcept : owner_(owner) {

}


void TextBoxRenderFacet::PaintText(
    Canvas& canvas,
    const zaf::Rect& dirty_rect,
    const dwrite::TextLayout& text_layout,
    const zaf::Rect& layout_rect) const {

    PaintCompositionUnderline(canvas, text_layout, layout_rect);
}


void TextBoxRenderFacet::PaintCompositionUnderline(
    Canvas& canvas,
    const dwrite::TextLayout& text_layout,
    const zaf::Rect& layout_rect) const {

    auto composition_range = owner_.editor_->GetCompositionRange();
    if (composition_range.IsEmpty()) {
        return;
    }

    auto state_gurad = canvas.PushState();
    canvas.SetBrushWithColor(owner_.TextColor());

    d2d::StrokeProperties stroke_properties;
    stroke_properties.SetDashStyle(d2d::Stroke::DashStyle::Dot);
    stroke_properties.SetDashCapStyle(d2d::Stroke::CapStyle::Round);
    stroke_properties.SetStartCapStyle(d2d::Stroke::CapStyle::Round);
    stroke_properties.SetEndCapStyle(d2d::Stroke::CapStyle::Round);
    auto stroke = GraphicFactory::Instance().CreateStroke(stroke_properties);
    canvas.SetStroke(stroke);

    auto range_metrics = text_layout.HitTestRange(composition_range);
    for (const auto& metrics : range_metrics) {

        constexpr float LineWidth = 1.0f;
        float y = metrics.Top() + metrics.Height() - LineWidth;

        Point start_point{ metrics.Left(), y };
        start_point.AddOffset(layout_rect.position);

        Point end_point{ metrics.Left() + metrics.Width(), y };
        end_point.AddOffset(layout_rect.position);

        canvas.DrawLine(start_point, end_point, LineWidth);
    }
}

}