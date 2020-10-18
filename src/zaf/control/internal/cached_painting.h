#pragma once

#include <zaf/graphic/rect.h>

namespace zaf::internal {

class CalculateDirtyRectResult {
public:
    Rect actual_dirty_rect;
    Rect new_valid_cached_rect;
};

CalculateDirtyRectResult CalculateDirtyRectOutsideValidCachedRect(
    const Rect& dirty_rect,
    const Rect& valid_cached_rect);

Rect CalculateInvalidRectInCachedRect(
    const Rect& valid_cached_rect, 
    const Rect& need_repaint_rect);

}