#pragma once

#include <zaf/control/image_layout.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/d2d/interpolation_mode.h>
#include <zaf/graphic/rect.h>
#include <zaf/graphic/d2d/render_bitmap.h>

namespace zaf::internal {

void DrawImage(
    Canvas& canvas,
    const Rect& draw_rect,
    ImageLayout image_layout,
    const d2d::RenderBitmap& bitmap,
    d2d::InterpolationMode interpolation_mode);

std::vector<Point> CalculateTiledImagePositions(const Rect& draw_rect, const Size& image_size);

}