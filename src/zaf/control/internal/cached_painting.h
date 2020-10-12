#pragma once

#include <zaf/graphic/rect.h>

namespace zaf::internal {

Rect CalculateInvalidRectInCachedRendereRect(
    const Rect& valid_cached_renderer_rect, 
    const Rect& need_repaint_rect);

}