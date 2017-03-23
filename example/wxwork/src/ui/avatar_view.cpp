#include "ui/avatar_view.h"
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/resource_factory.h>

void AvatarView::Paint(zaf::Canvas& canvas, const zaf::Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    if (image_source_ == nullptr) {
        return;
    }

    auto bitmap = canvas.GetRenderer().CreateBitmap(image_source_);
    if (bitmap == nullptr) {
        return;
    }

    canvas.DrawBitmap(bitmap, GetContentRect());
}