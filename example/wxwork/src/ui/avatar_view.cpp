#include "ui/avatar_view.h"
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/resource_factory.h>

void AvatarView::Paint(zaf::Canvas& canvas, const zaf::Rect& dirty_rect) {

    if (image_source_ == nullptr) {
        return;
    }

    auto bitmap = canvas.GetRenderer().CreateBitmap(image_source_);
    if (bitmap == nullptr) {
        return;
    }

    auto bitmap_brush = canvas.GetRenderer().CreateBitmapBrush(bitmap);
    if (bitmap_brush == nullptr) {
        return;
    }

    zaf::RoundedRect rounded_rect;
    rounded_rect.rect = GetContentRect();
    rounded_rect.x_radius = 4;
    rounded_rect.y_radius = 4;

    auto rounded_rect_geometry = zaf::GetResourceFactory()->CreateRoundedRectangleGeometry(rounded_rect);
    if (rounded_rect_geometry == nullptr) {
        return;
    }

    zaf::Canvas::StateGuard state_guard(canvas);

    canvas.SetBrush(bitmap_brush);
    canvas.DrawGeometry(rounded_rect_geometry);
}