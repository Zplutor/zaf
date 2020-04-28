#pragma once

#include <zaf/control/image_layout.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/interpolation_mode.h>
#include <zaf/graphic/rect.h>
#include <zaf/graphic/render_bitmap.h>

namespace zaf::internal {

void DrawImage(
    Canvas& canvas,
    const Rect& draw_rect,
    ImageLayout image_layout,
    const RenderBitmap& bitmap,
    InterpolationMode interpolation_mode);

std::vector<Point> CalculateTiledImagePositions(const Rect& draw_rect, const Size& image_size);

}