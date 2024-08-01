#include "ui/avatar_view.h"
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/graphic_factory.h>

void AvatarView::Paint(zaf::Canvas& canvas, const zaf::Rect& dirty_rect) const {

    if (!image_source_) {
        return;
    }

    auto bitmap = canvas.Renderer().CreateBitmap(image_source_);
    if (!bitmap) {
        return;
    }

    auto bitmap_brush = canvas.Renderer().CreateBitmapBrush(bitmap);
    if (!bitmap_brush) {
        return;
    }

    zaf::RoundedRect rounded_rect;
    rounded_rect.rect = ContentRect();
    rounded_rect.x_radius = 4;
    rounded_rect.y_radius = 4;

    auto rounded_rect_geometry = 
        zaf::GraphicFactory::Instance().CreateRoundedRectangleGeometry(rounded_rect);

    if (!rounded_rect_geometry) {
        return;
    }

    auto state_guard = canvas.PushState();
    canvas.SetBrush(bitmap_brush);
    canvas.DrawGeometry(rounded_rect_geometry);
}