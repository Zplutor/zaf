#pragma once

#include <zaf/graphic/rect.h>

namespace zaf::internal {

/**
Note: all rects are in canvas coordinate, rather in upper layer coordinate.
*/
class CanvasLayer {
public:
    Rect rect;
    Rect aligned_rect;
    Rect paintable_rect;
    Rect aligned_paintable_rect;
};

}