#include <zaf/control/internal/cached_painting.h>

namespace zaf::internal {

Rect CalculateInvalidRectInCachedRendereRect(
    const Rect& valid_cached_renderer_rect,
    const Rect& need_repaint_rect) {

    Rect intersection = Rect::Intersect(valid_cached_renderer_rect, need_repaint_rect);
    if (intersection.IsEmpty()) {
        return {};
    }

    if (intersection == valid_cached_renderer_rect) {
        return intersection;
    }

    float left = intersection.position.x - valid_cached_renderer_rect.position.x;
    float top = intersection.position.y - valid_cached_renderer_rect.position.y;
    float right =
        (valid_cached_renderer_rect.position.x + valid_cached_renderer_rect.size.width) -
        (intersection.position.x + intersection.size.width);
    float bottom =
        (valid_cached_renderer_rect.position.y + valid_cached_renderer_rect.size.height) -
        (intersection.position.y + intersection.size.height);

    float max = (std::max)({ left, top, right, bottom });
    if (max == 0) {
        return {};
    }

    Rect result;
    if (max == left) {

        result.position.x = intersection.position.x;
        result.position.y = valid_cached_renderer_rect.position.y;
        result.size.width =
            valid_cached_renderer_rect.position.x + valid_cached_renderer_rect.size.width -
            intersection.position.x;
        result.size.height = valid_cached_renderer_rect.size.height;
    }
    else if (max == top) {

        result.position.x = valid_cached_renderer_rect.position.x;
        result.position.y = intersection.position.y;
        result.size.width = valid_cached_renderer_rect.size.width;
        result.size.height =
            valid_cached_renderer_rect.position.y + valid_cached_renderer_rect.size.height -
            intersection.position.y;
    }
    else if (max == right) {

        result.position.x = valid_cached_renderer_rect.position.x;
        result.position.y = valid_cached_renderer_rect.position.y;
        result.size.width =
            intersection.position.x + intersection.size.width -
            valid_cached_renderer_rect.position.x;
        result.size.height = valid_cached_renderer_rect.size.height;
    }
    else if (max == bottom) {

        result.position.x = valid_cached_renderer_rect.position.x;
        result.position.y = valid_cached_renderer_rect.position.y;
        result.size.width = valid_cached_renderer_rect.size.width;
        result.size.height =
            intersection.position.y + intersection.size.height -
            valid_cached_renderer_rect.position.y;
    }
    return result;
}

}