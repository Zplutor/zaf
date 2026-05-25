#include <zaf/control/internal/text_box/text_box_render_facet.h>
#include <zaf/control/internal/textual/text_box_editor.h>
#include <zaf/graphic/canvas/canvas.h>
#include <zaf/graphic/d2d/stroke_style_properties.h>
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
    state_gurad.SetBrush(owner_.TextColor());

    d2d::StrokeStyleProperties stroke_properties;
    stroke_properties.SetDashStyle(d2d::DashStyle::Dot);
    stroke_properties.SetDashCap(d2d::LineCap::Round);
    stroke_properties.SetStartCap(d2d::LineCap::Round);
    stroke_properties.SetEndCap(d2d::LineCap::Round);
    auto stroke_style = GraphicFactory::Instance().CreateStrokeStyle(stroke_properties);
    state_gurad.SetStrokeStyle(stroke_style);

    constexpr float LineWidth = 1.0f;
    state_gurad.SetStrokeWidth(LineWidth);

    auto range_metrics = text_layout.HitTestRange(composition_range);
    for (const auto& metrics : range_metrics) {

        float y = metrics.Top() + metrics.Height() - LineWidth;

        Point start_point{ metrics.Left(), y };
        start_point.AddOffset(layout_rect.position);

        Point end_point{ metrics.Left() + metrics.Width(), y };
        end_point.AddOffset(layout_rect.position);

        canvas.DrawLine(start_point, end_point);
    }
}

}