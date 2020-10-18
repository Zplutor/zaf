#include <zaf/control/internal/cached_painting.h>

namespace zaf::internal {

CalculateDirtyRectResult CalculateDirtyRectOutsideValidCachedRect(
    const Rect& dirty_rect,
    const Rect& valid_cached_rect) {

    CalculateDirtyRectResult result;
    
    //Calculate the dirty rect that is not in valid cached rect.
    result.actual_dirty_rect = Rect::Subtract(dirty_rect, valid_cached_rect);

    //Whole dirty rect is in valid cached rect, retain valid cached rect.
    if (result.actual_dirty_rect.IsEmpty()) {
        result.new_valid_cached_rect = valid_cached_rect;
        return result;
    }

    //Whole dirty rect is dirty, reset valid cached rect to the whole dirty rect.
    if (result.actual_dirty_rect == dirty_rect) {
        result.new_valid_cached_rect = result.actual_dirty_rect;
        return result;
    }

    //There is a new dirty rect needs to repaint, union to the valid cached rect.
    result.new_valid_cached_rect = Rect::Union(valid_cached_rect, result.actual_dirty_rect);
    return result;
}


Rect CalculateInvalidRectInCachedRect(
    const Rect& valid_cached_rect,
    const Rect& need_repaint_rect) {

    Rect intersection = Rect::Intersect(valid_cached_rect, need_repaint_rect);
    if (intersection.IsEmpty()) {
        return {};
    }

    if (intersection == valid_cached_rect) {
        return intersection;
    }

    float left = intersection.position.x - valid_cached_rect.position.x;
    float top = intersection.position.y - valid_cached_rect.position.y;
    float right =
        (valid_cached_rect.position.x + valid_cached_rect.size.width) -
        (intersection.position.x + intersection.size.width);
    float bottom =
        (valid_cached_rect.position.y + valid_cached_rect.size.height) -
        (intersection.position.y + intersection.size.height);

    float max = (std::max)({ left, top, right, bottom });
    if (max == 0) {
        return {};
    }

    Rect result;
    if (max == left) {

        result.position.x = intersection.position.x;
        result.position.y = valid_cached_rect.position.y;
        result.size.width =
            valid_cached_rect.position.x + valid_cached_rect.size.width -
            intersection.position.x;
        result.size.height = valid_cached_rect.size.height;
    }
    else if (max == top) {

        result.position.x = valid_cached_rect.position.x;
        result.position.y = intersection.position.y;
        result.size.width = valid_cached_rect.size.width;
        result.size.height =
            valid_cached_rect.position.y + valid_cached_rect.size.height -
            intersection.position.y;
    }
    else if (max == right) {

        result.position.x = valid_cached_rect.position.x;
        result.position.y = valid_cached_rect.position.y;
        result.size.width =
            intersection.position.x + intersection.size.width -
            valid_cached_rect.position.x;
        result.size.height = valid_cached_rect.size.height;
    }
    else if (max == bottom) {

        result.position.x = valid_cached_rect.position.x;
        result.position.y = valid_cached_rect.position.y;
        result.size.width = valid_cached_rect.size.width;
        result.size.height =
            intersection.position.y + intersection.size.height -
            valid_cached_rect.position.y;
    }
    return result;
}

}