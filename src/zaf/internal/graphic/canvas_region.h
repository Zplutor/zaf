#pragma once

#include <zaf/graphic/rect.h>

namespace zaf::internal {

/**
Note: all rects are in canvas coordinate, rather in upper region coordinate.
*/
class CanvasRegion {
public:
    /**
    Gets the offset of the snapped rectangle to the original rectangle.
    */
    Point SnappedOffset() const noexcept {
        auto result = snapped_rect.position;
        result.SubtractOffset(rect.position);
        return result;
    }

    /**
    The rectangle area of the region.
    */
    Rect rect;

    /**
    The rectangle area of the region after snapping to pixel grid.
    */
    Rect snapped_rect;

    Rect paintable_rect;
    Rect snapped_paintable_rect;
};

}